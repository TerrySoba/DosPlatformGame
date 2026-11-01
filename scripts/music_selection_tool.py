#!/usr/bin/env python3
"""GUI tool to select the music of each level screen on the world map.

The music is stored as the "music" map property in the levels/*.tmx files.
After saving, the .map files have to be regenerated (make -C levels).
"""

import argparse
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

from PySide6.QtCore import Qt, QProcess, QProcessEnvironment, QRectF
from PySide6.QtGui import (QAction, QBrush, QColor, QFont, QImage, QKeySequence,
                           QPainter, QPen, QPixmap, QIcon, qRgb)
from PySide6.QtWidgets import (QApplication, QComboBox, QFormLayout, QGraphicsRectItem,
                               QGraphicsScene, QGraphicsSimpleTextItem, QGraphicsView,
                               QHBoxLayout, QLabel, QMainWindow, QMessageBox, QPushButton,
                               QSplitter, QVBoxLayout, QWidget)

REPO_DIR = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO_DIR / "WorldMapBuilder"))
import image  # noqa: E402

SCREEN_WIDTH = 320
SCREEN_HEIGHT = 200
MUSIC_INDEX_NO_CHANGE = 0
MUSIC_INDEX_NO_MUSIC = 1

MUSIC_DIR = REPO_DIR / "music"
RADPLAY_DIR = REPO_DIR / "tools" / "dos"
DOSBOX_LAUNCHER = REPO_DIR / "dosbox_launcher.sh"
PLAY_MUSIC_DOSBOX_CONFIG = REPO_DIR / "scripts" / "play_music.dosbox_config"

LEVEL_FILE_PATTERN = re.compile(r"^([0-9a-fA-F]{2})([0-9a-fA-F]{2})(b1)?\.tmx$")
MUSIC_PROPERTY_PATTERN = re.compile(r'<property\b[^>]*\bname="music"[^>]*/>')


def read_music_names(header_file):
    """Read the music index names from the MusicIndex enum in music_controller.h."""
    names = {}
    for match in re.finditer(r"MUSIC_INDEX_(\w+)\s*=\s*(\d+)", header_file.read_text()):
        names[int(match.group(2))] = match.group(1)
    return names


def read_music_files(controller_file):
    """Read the module file names per music index name from the switch statement in
    music_controller_dos.cpp."""
    return {match.group(1): match.group(2) for match in re.finditer(
        r'case\s+MUSIC_INDEX_(\w+)\s*:\s*modulePath\s*=\s*"([^"]+)"', controller_file.read_text())}


def read_music_info(header_file, controller_file):
    """Return a readable name and the music file name (if there is one) per music index."""
    files_by_name = read_music_files(controller_file)
    names = {}
    files = {}
    for index, name in read_music_names(header_file).items():
        names[index] = name.replace("_", " ").capitalize()
        if name in files_by_name:
            files[index] = files_by_name[name]
    return names, files


def music_color(index, count):
    """Color used to mark screens with the given music index."""
    if isinstance(index, str) or index == MUSIC_INDEX_NO_CHANGE:
        return None
    if index == MUSIC_INDEX_NO_MUSIC:
        return QColor(128, 128, 128)
    first = MUSIC_INDEX_NO_MUSIC + 1
    return QColor.fromHsv(int((index - first) * 360 / max(1, count - first)), 255, 255)


def read_tmx_music(tmx_file):
    """Read the music property of a tmx file, the same way MapTool does."""
    element = ET.parse(tmx_file).getroot().find("./properties/property[@name='music']")
    if element is not None:
        try:
            return int(element.attrib["value"])
        except (KeyError, ValueError):
            pass
    return MUSIC_INDEX_NO_CHANGE


def _remove_line(text, start, end):
    """Remove text[start:end] including its whole line, if nothing else is on that line."""
    line_start = text.rfind("\n", 0, start) + 1
    line_end = text.find("\n", end)
    line_end = len(text) if line_end == -1 else line_end + 1
    if text[line_start:start].strip() == "" and text[end:line_end].strip() == "":
        return text[:line_start] + text[line_end:]
    return text[:start] + text[end:]


def set_tmx_music(text, music):
    """Return the tmx file content with the map music property set to music.

    The file is edited textually, so the formatting written by Tiled (line endings,
    indentation, attribute order) is kept. MUSIC_INDEX_NO_CHANGE removes the property.
    """
    newline = "\r\n" if "\r\n" in text else "\n"
    property_tag = '<property name="music" type="int" value="{}"/>'.format(music)

    map_tag = re.search(r"<map\b[^>]*>", text)
    # Map properties come before the first tileset/layer element
    first_child = re.compile(r"<(tileset|layer|objectgroup|imagelayer|group)\b").search(text, map_tag.end())
    header_end = first_child.start() if first_child else len(text)
    properties = re.compile(r"<properties>(.*?)</properties>", re.S).search(text, map_tag.end(), header_end)

    if properties is None:
        if music == MUSIC_INDEX_NO_CHANGE:
            return text
        line_start = text.rfind("\n", 0, header_end) + 1
        indent = text[line_start:header_end]
        block = (indent + "<properties>" + newline +
                 indent + " " + property_tag + newline +
                 indent + "</properties>" + newline)
        return text[:line_start] + block + text[line_start:]

    music_property = MUSIC_PROPERTY_PATTERN.search(text, properties.start(1), properties.end(1))
    if music == MUSIC_INDEX_NO_CHANGE:
        if music_property is None:
            return text
        text = _remove_line(text, music_property.start(), music_property.end())
        # Remove the properties element if it is empty now
        properties = re.compile(r"<properties>(.*?)</properties>", re.S).search(text, map_tag.end())
        if properties.group(1).strip() == "":
            text = _remove_line(text, properties.start(), properties.end())
        return text

    if music_property is not None:
        return text[:music_property.start()] + property_tag + text[music_property.end():]

    close_tag = properties.end() - len("</properties>")
    line_start = text.rfind("\n", 0, close_tag) + 1
    indent = text[line_start:close_tag] + " "
    return text[:line_start] + indent + property_tag + newline + text[line_start:]


class Level:
    def __init__(self, tmx_file):
        self.tmx_file = tmx_file
        self.music = read_tmx_music(tmx_file)
        self.saved_music = self.music

    @property
    def modified(self):
        return self.music != self.saved_music

    def save(self):
        if isinstance(self.music, str):
            raise ValueError("{} has no music index in music_controller_dos.cpp, "
                             "so it can not be saved".format(self.music))
        with open(self.tmx_file, newline="") as f:
            text = f.read()
        with open(self.tmx_file, "w", newline="") as f:
            f.write(set_tmx_music(text, self.music))
        self.saved_music = self.music


class Screen:
    """One screen of the world map with its level and the optional button variant (b1)."""

    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.levels = {}  # variant suffix ("" or "b1") -> Level
        self.label_items = []
        self.tint_item = None

    @property
    def name(self):
        return "{:02X}{:02X}".format(self.x + 1, self.y + 1)

    def rect(self):
        return QRectF(self.x * SCREEN_WIDTH, self.y * SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT)


def load_screens(levels_dir):
    screens = {}
    for tmx_file in sorted(levels_dir.glob("*.tmx")):
        match = LEVEL_FILE_PATTERN.match(tmx_file.name)
        if not match:
            continue
        pos = (int(match.group(1), 16) - 1, int(match.group(2), 16) - 1)
        screen = screens.setdefault(pos, Screen(*pos))
        screen.levels[match.group(3) or ""] = Level(tmx_file)
    return screens


def load_world_image(tga_dir, png_file):
    """Compose the world map from the level images dumped by the game (these have no
    music labels drawn on them), or fall back to the pregenerated world.png."""
    tga_files = sorted(tga_dir.glob("*.TGA")) if tga_dir.is_dir() else []
    positions = []
    for tga_file in tga_files:
        match = re.match(r"^([0-9a-fA-F]{2})([0-9a-fA-F]{2})\.TGA$", tga_file.name)
        if match:
            positions.append((int(match.group(1), 16) - 1, int(match.group(2), 16) - 1, tga_file))

    if not positions:
        return QImage(str(png_file))

    width = (max(p[0] for p in positions) + 1) * SCREEN_WIDTH
    height = (max(p[1] for p in positions) + 1) * SCREEN_HEIGHT
    world = image.Image(width, height)
    for x, y, tga_file in positions:
        screen_image = image.Image(1, 1)
        screen_image.loadFromTga(str(tga_file))
        world.drawImage(screen_image, x * SCREEN_WIDTH, y * SCREEN_HEIGHT)

    qimage = QImage(bytes(world._data), width, height, width, QImage.Format_Indexed8)
    # The palette is stored in TGA order (blue, green, red)
    palette = image.rgbiColors
    qimage.setColorTable([qRgb(palette[i + 2], palette[i + 1], palette[i])
                          for i in range(0, len(palette), 3)])
    return qimage.copy()  # detach from the temporary bytes object


class WorldMapView(QGraphicsView):
    def __init__(self, scene, on_click):
        super().__init__(scene)
        self._on_click = on_click
        self._press_pos = None
        self.setDragMode(QGraphicsView.ScrollHandDrag)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setRenderHint(QPainter.SmoothPixmapTransform, False)
        self.setBackgroundBrush(QColor(40, 40, 40))

    def wheelEvent(self, event):
        factor = 1.25 if event.angleDelta().y() > 0 else 1 / 1.25
        self.scale(factor, factor)

    def mousePressEvent(self, event):
        self._press_pos = event.position().toPoint()
        super().mousePressEvent(event)

    def mouseReleaseEvent(self, event):
        super().mouseReleaseEvent(event)
        # Only treat it as a click if the mouse was not dragged to scroll the map
        if (event.button() == Qt.LeftButton and self._press_pos is not None and
                (event.position().toPoint() - self._press_pos).manhattanLength() < 5):
            self._on_click(self.mapToScene(self._press_pos))
        self._press_pos = None


class MainWindow(QMainWindow):
    def __init__(self, screens, world_image, music_names, music_files):
        super().__init__()
        self.screens = screens
        self.music_names = music_names
        self.music_files = music_files
        self.selected = None
        self.combos = {}
        self.player = None

        self.scene = QGraphicsScene(self)
        self.scene.addPixmap(QPixmap.fromImage(world_image))

        self.label_font = QFont("Sans", 18, QFont.Bold)
        for screen in screens.values():
            screen.tint_item = self.scene.addRect(screen.rect(), QPen(Qt.NoPen))
            self.update_screen(screen)

        self.selection_item = self.scene.addRect(QRectF(), QPen(QColor(255, 255, 0), 4))
        self.selection_item.setZValue(10)
        self.selection_item.hide()

        self.view = WorldMapView(self.scene, self.on_map_clicked)

        self.panel = QWidget()
        panel_layout = QVBoxLayout(self.panel)
        self.title_label = QLabel("Click on a screen to select it.")
        self.title_label.setWordWrap(True)
        panel_layout.addWidget(self.title_label)
        self.form = QFormLayout()
        panel_layout.addLayout(self.form)
        self.stop_button = QPushButton("Stop music", clicked=self.stop_music, enabled=False)
        panel_layout.addWidget(self.stop_button)
        panel_layout.addStretch()
        legend = QLabel("Wheel: zoom, drag: scroll<br>"
                        "Screens without a color keep the music of the previous screen.<br>"
                        "After saving, rebuild the .map files (make -C levels).")
        legend.setWordWrap(True)
        panel_layout.addWidget(legend)

        splitter = QSplitter()
        splitter.addWidget(self.view)
        splitter.addWidget(self.panel)
        splitter.setStretchFactor(0, 1)
        splitter.setSizes([1200, 300])
        self.setCentralWidget(splitter)

        file_menu = self.menuBar().addMenu("&File")
        save_action = QAction("&Save", self, shortcut=QKeySequence.Save, triggered=self.save)
        quit_action = QAction("&Quit", self, shortcut=QKeySequence.Quit, triggered=self.close)
        file_menu.addAction(save_action)
        file_menu.addAction(quit_action)
        view_menu = self.menuBar().addMenu("&View")
        view_menu.addAction(QAction("&Fit to window", self, shortcut="Ctrl+0", triggered=self.fit))
        view_menu.addAction(QAction("&Actual size", self, shortcut="Ctrl+1",
                                    triggered=self.view.resetTransform))

        self.update_title()
        self.resize(1500, 900)

    def showEvent(self, event):
        super().showEvent(event)
        self.fit()

    def fit(self):
        self.view.fitInView(self.scene.sceneRect(), Qt.KeepAspectRatio)

    def music_name(self, index):
        if isinstance(index, str):
            return index  # music file without a music index
        return self.music_names.get(index, "Unknown ({})".format(index))

    def music_name_with_file(self, index):
        if isinstance(index, str):
            return "{} (no music index, can not be saved)".format(index)
        if index in self.music_files:
            return "{} ({})".format(self.music_name(index), self.music_files[index])
        return self.music_name(index)

    def music_icon(self, index):
        pixmap = QPixmap(16, 16)
        color = music_color(index, len(self.music_names))
        pixmap.fill(color if color else Qt.transparent)
        return QIcon(pixmap)

    def update_screen(self, screen):
        """Update the tint and the music labels of a screen on the map."""
        base = screen.levels.get("")
        color = music_color(base.music, len(self.music_names)) if base else None
        if color:
            color.setAlpha(70)
            screen.tint_item.setBrush(QBrush(color))
        else:
            screen.tint_item.setBrush(Qt.NoBrush)

        for item in screen.label_items:
            self.scene.removeItem(item)
        screen.label_items = []

        y = screen.y * SCREEN_HEIGHT + 4
        for variant, level in sorted(screen.levels.items()):
            # Only label screens that change the music, or unsaved changes back to "no change"
            if level.music == MUSIC_INDEX_NO_CHANGE and not level.modified:
                continue
            text = (variant + ": " if variant else "") + self.music_name(level.music)
            if level.modified:
                text += " *"
            if level.music in self.music_files:
                text += "\n" + self.music_files[level.music]
            text_item = QGraphicsSimpleTextItem(text)
            text_item.setFont(self.label_font)
            if level.music == MUSIC_INDEX_NO_CHANGE:
                text_color = QColor(255, 255, 255)
            elif level.music == MUSIC_INDEX_NO_MUSIC:
                text_color = QColor(255, 85, 85)
            elif isinstance(level.music, str):
                text_color = QColor(255, 170, 0)
            else:
                text_color = QColor(85, 255, 85)
            text_item.setBrush(text_color)
            text_item.setPos(screen.x * SCREEN_WIDTH + 8, y)
            text_item.setZValue(2)
            box = text_item.boundingRect().adjusted(-4, -2, 4, 2).translated(text_item.pos())
            box_item = self.scene.addRect(box, QPen(Qt.NoPen), QBrush(QColor(0, 0, 0)))
            box_item.setZValue(1)
            self.scene.addItem(text_item)
            screen.label_items += [box_item, text_item]
            y += box.height() + 2

    def on_map_clicked(self, pos):
        key = (int(pos.x() // SCREEN_WIDTH), int(pos.y() // SCREEN_HEIGHT))
        if pos.x() >= 0 and pos.y() >= 0 and key in self.screens:
            self.select(self.screens[key])

    def select(self, screen):
        self.selected = screen
        self.selection_item.setRect(screen.rect())
        self.selection_item.show()
        self.title_label.setText("<b>Screen {}</b>".format(screen.name))

        while self.form.rowCount():
            self.form.removeRow(0)
        self.combos = {}
        for variant, level in sorted(screen.levels.items()):
            combo = QComboBox()
            indices = sorted(self.music_names)
            if isinstance(level.music, int) and level.music not in self.music_names:
                indices.append(level.music)
            for index in indices:
                combo.addItem(self.music_icon(index), "{} - {}".format(index, self.music_name_with_file(index)), index)
            # Music files that are not used by any music index can be selected to play them
            other_files = self.unassigned_music_files()
            if isinstance(level.music, str) and level.music not in other_files:
                other_files.append(level.music)
            if other_files:
                combo.insertSeparator(combo.count())
            for file_name in other_files:
                combo.addItem(self.music_icon(file_name), self.music_name_with_file(file_name), file_name)
            combo.setCurrentIndex(combo.findData(level.music))
            combo.currentIndexChanged.connect(
                lambda _, c=combo, l=level: self.set_music(screen, l, c.currentData()))
            play_button = QPushButton("Play")
            play_button.setToolTip("Play the selected music in DOSBox using RADPLAY.EXE")
            play_button.clicked.connect(lambda _=False, c=combo: self.play_music(c.currentData()))
            update_play_button = lambda _=None, c=combo, b=play_button: b.setEnabled(
                self.music_file_path(c.currentData()) is not None)
            combo.currentIndexChanged.connect(update_play_button)
            update_play_button()
            row = QHBoxLayout()
            row.addWidget(combo, 1)
            row.addWidget(play_button)
            self.form.addRow(level.tmx_file.name, row)
            self.combos[variant] = combo

    def unassigned_music_files(self):
        """Names of the .rad files in the music directory that no music index uses."""
        assigned = {name.lower() for name in self.music_files.values()}
        return [path.name for path in sorted(MUSIC_DIR.iterdir())
                if path.suffix.lower() == ".rad" and path.name.lower() not in assigned and path.is_file()]

    def music_file_path(self, music):
        """Path of the .rad file of the music index, or None if there is none.
        The file name is matched case-insensitively, like DOS does.
        music may also be the name of a file in the music directory."""
        if isinstance(music, str):
            path = MUSIC_DIR / music
            return path if path.is_file() else None
        if music not in self.music_files:
            return None
        path = MUSIC_DIR / self.music_files[music]
        if path.is_file():
            return path
        name = path.name.lower()
        for candidate in sorted(MUSIC_DIR.iterdir()):
            if candidate.name.lower() == name and candidate.is_file():
                return candidate
        return None

    def play_music(self, music):
        """Play the music in DOSBox with RADPLAY.EXE. The music dir is drive C:, the
        player is on drive D:. DOSBox runs without a window (SDL dummy video driver),
        so the music plays until stop_music() is called."""
        path = self.music_file_path(music)
        if path is None:
            return
        self.stop_music()
        self.player = QProcess(self)
        environment = QProcessEnvironment.systemEnvironment()
        environment.insert("SDL_VIDEODRIVER", "dummy")
        self.player.setProcessEnvironment(environment)
        self.player.finished.connect(self.on_player_finished)
        self.player.errorOccurred.connect(self.on_player_error)
        self.player.start("bash", [str(DOSBOX_LAUNCHER), str(PLAY_MUSIC_DOSBOX_CONFIG), str(MUSIC_DIR),
                                   'mount d "{}"'.format(RADPLAY_DIR),
                                   "D:\\RADPLAY.EXE {}".format(path.name),
                                   "exit"])
        self.stop_button.setEnabled(True)
        self.statusBar().showMessage("Playing {}".format(path.name))

    def stop_music(self):
        if self.player is None:
            return
        player = self.player
        self.player = None
        player.finished.disconnect(self.on_player_finished)
        player.errorOccurred.disconnect(self.on_player_error)
        # Killing the launcher script would leave DOSBox running, so kill its children first
        QProcess.execute("pkill", ["-P", str(player.processId())])
        player.kill()
        player.waitForFinished(2000)
        self.stop_button.setEnabled(False)
        self.statusBar().clearMessage()

    def on_player_finished(self):
        self.player = None
        self.stop_button.setEnabled(False)
        self.statusBar().clearMessage()

    def on_player_error(self, error):
        if error == QProcess.FailedToStart:
            QMessageBox.critical(self, "Playing failed",
                                 "Could not start {}: {}".format(DOSBOX_LAUNCHER, self.player.errorString()))
            self.player = None
            self.stop_button.setEnabled(False)
            self.statusBar().clearMessage()

    def set_music(self, screen, level, music):
        level.music = music
        self.update_screen(screen)
        self.update_title()

    def modified_levels(self):
        return [level for screen in self.screens.values()
                for level in screen.levels.values() if level.modified]

    def update_title(self):
        modified = len(self.modified_levels())
        self.setWindowTitle("Music Selection Tool" + (" - {} unsaved".format(modified) if modified else ""))

    def save(self):
        failed = []
        for screen in self.screens.values():
            for level in screen.levels.values():
                if level.modified:
                    try:
                        level.save()
                    except Exception as e:
                        failed.append("{}: {}".format(level.tmx_file.name, e))
            self.update_screen(screen)
        self.update_title()
        if failed:
            QMessageBox.critical(self, "Save failed", "\n".join(failed))
            return False
        self.statusBar().showMessage("Saved. Rebuild the .map files to use the new music.", 5000)
        return True

    def closeEvent(self, event):
        if self.modified_levels():
            answer = QMessageBox.question(
                self, "Unsaved changes", "Save the changed levels before quitting?",
                QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel)
            if answer == QMessageBox.Cancel or (answer == QMessageBox.Save and not self.save()):
                event.ignore()
                return
        self.stop_music()
        event.accept()


def main():
    parser = argparse.ArgumentParser(description="Select the music for each screen of the world map.")
    parser.add_argument("--levels", type=Path, default=REPO_DIR / "levels",
                        help="directory containing the level .tmx files")
    parser.add_argument("--tga-dir", type=Path, default=REPO_DIR / "release",
                        help="directory containing the level images dumped by the game")
    parser.add_argument("--world-image", type=Path, default=REPO_DIR / "artifacts" / "world.png",
                        help="world map image used if no level images are found in --tga-dir")
    args, qt_args = parser.parse_known_args()

    app = QApplication([sys.argv[0]] + qt_args)
    music_names, music_files = read_music_info(
        REPO_DIR / "source" / "music_controller.h",
        REPO_DIR / "source" / "platform" / "dos" / "music_controller_dos.cpp")
    screens = load_screens(args.levels)
    world_image = load_world_image(args.tga_dir, args.world_image)
    if world_image.isNull():
        QMessageBox.warning(None, "No world map",
                            "Neither level images in {} nor {} were found.".format(args.tga_dir, args.world_image))

    window = MainWindow(screens, world_image, music_names, music_files)
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
