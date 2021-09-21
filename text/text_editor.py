import json, sys
from PyQt5 import QtWidgets, QtGui, uic
import os.path

script_dir = os.path.dirname(os.path.realpath(__file__))

class StringFile:
    def __init__(self):
        self._entries = dict()
        self._edited = False
        pass
    
    def load(self, jsonFileName):
        with open(jsonFileName, "rb") as fp:
            self._entries = json.loads(fp.read())
 
    def save(self, jsonFileName):
        with open(jsonFileName, "wb") as fp:
            fp.write(json.dumps(self._entries, indent=4).encode("utf8"))
        self._edited = False

    def getIdList(self):
        return list(self._entries)

    def getEntry(self, id):
        return self._entries[id]

    def newEntry(self):
        self._edited = True
        id = 1
        while str(id) in self._entries:
            id = id + 1
        self._entries[str(id)] = dict(en = "", de = "")
        return str(id)

    def setEntry(self, id, entry):
        oldEntry = self._entries[id]
        self._entries[id] = entry
        if repr(oldEntry) != repr(self._entries[id]):
            self._edited = True

    def renameEntry(self, oldId, newId):
        self._edited = True
        entry = self._entries[oldId]
        del self._entries[oldId]
        self._entries[newId] = entry

    def deleteEntry(self, id):
        self._edited = True
        del self._entries[id]

    def isEdited(self):
        return self._edited

class Ui(QtWidgets.QMainWindow):
    def __init__(self, filename = None):
        """If filename is given, then the file with the given filename is opened immediately."""
        super(Ui, self).__init__() # Call the inherited classes __init__ method
        self._appName = "Translation Editor"
        self._selectedId = None
        self._filename = None
        self._uiLocked = False
        self._strings = StringFile()
        
        uic.loadUi(script_dir + '/text_editor.ui', self) # Load the .ui file
        self.show() # Show the GUI
        self.idList.clear()

        self.actionAbout.triggered.connect(self.displayAboutMessage)
        self.actionQuit.triggered.connect(self.quit)
        self.actionOpen.triggered.connect(self.openFile)
        self.actionSave_As.triggered.connect(self.saveAs)
        self.actionSave.triggered.connect(self.save)
        self.actionNew.triggered.connect(self.new)
        self.addIdButton.clicked.connect(self.addNewEntry)
        self.englishTextEdit.textChanged.connect(self.contentChanged)
        self.germanTextEdit.textChanged.connect(self.contentChanged)
        self.idList.currentTextChanged.connect(self.idSelected)
        self.renameIdButton.clicked.connect(self.renameIdClicked)
        self.removeIdButton.clicked.connect(self.removeIdClicked)

        self.updateUi()

        if filename is not None:
            self.load(filename)

    def closeEvent(self, event):
        if self._filename is None:
            event.accept()
            return

        filename = os.path.basename(self._filename)

        selection = QtWidgets.QMessageBox.question(self, "Save changes?", "Save changes to unsaved file \"{}\"?".format(filename),
            QtWidgets.QMessageBox.Discard | QtWidgets.QMessageBox.Cancel | QtWidgets.QMessageBox.Save, QtWidgets.QMessageBox.Save)
        
        if selection == QtWidgets.QMessageBox.Cancel:
            event.ignore()
            return

        if selection == QtWidgets.QMessageBox.Save:
            self.save()

        event.accept()

    def removeIdClicked(self):
        if self._selectedId is not None and len(self._selectedId) > 0:
            self._strings.deleteEntry(self._selectedId)
            self._selectedId = None
            ids = self._strings.getIdList()
            if len(ids) > 0:
                self._selectedId = ids[0]
            self.updateUi()

    def renameIdClicked(self):
        newId = self.idLineEdit.text()
        if len(newId) > 0:
            self._strings.renameEntry(self._selectedId, newId)
            self._selectedId = newId
            self.updateUi()

    def idSelected(self, id):
        if len(id) > 0:
            self._selectedId = id
            self.updateUi()

    def addNewEntry(self):
        id = self._strings.newEntry()
        self._selectedId = id
        self.updateUi()

    def quit(self):
        self.close()

    def openFile(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self,'Open Translation File','./','JSON Files(*.json)')
        if len(filename[0]) > 0:
            self.load(filename[0])
            self.updateUi()

    def saveAs(self):
        filename = QtWidgets.QFileDialog.getSaveFileName(self,'Save Translation File','./','JSON Files(*.json)')
        if len(filename[0]) > 0:
            self._strings.save(filename[0])
            self._filename = filename[0]
            self.updateUi()
    
    def save(self):
        if len(self._filename) > 0:
            self._strings.save(self._filename)
            self.updateUi()

    def new(self):
        self._strings = StringFile()
        self._selectedId = None
        self._filename = None
        self.updateUi()

    def displayAboutMessage(self):
        QtWidgets.QMessageBox.about(self, self._appName, "{} - An editor for translations for TandyGame".format(self._appName))
        pass

    def load(self, jsonFileName):
        self._filename = jsonFileName
        self._strings.load(jsonFileName)
        try:
            self._selectedId = self._strings.getIdList()[0]
        except KeyError:
            self._selectedId = None

        self.updateUi()
    
    def contentChanged(self):
        if self._selectedId is not None:
            entry = dict(en = self.englishTextEdit.toPlainText(), de = self.germanTextEdit.toPlainText())
            if not self._uiLocked:
                self._strings.setEntry(self._selectedId, entry)

    def updateUi(self):
        self._uiLocked = True

        if self._strings.isEdited():
            editedString = "*"
        else:
            editedString = ""

        if self._filename is None:
            self.actionSave.setEnabled(False)
            self.setWindowTitle(editedString + self._appName)
        else:
            self.setWindowTitle("{}{} - {}".format(editedString, os.path.basename(self._filename), self._appName))
            self.actionSave.setEnabled(True)

        if self._selectedId is None:
            self.idLineEdit.clear()
            self.englishTextEdit.clear()
            self.germanTextEdit.clear()
            self.idLineEdit.setEnabled(False)
            self.englishTextEdit.setEnabled(False)
            self.germanTextEdit.setEnabled(False)
        else:
            entry = self._strings.getEntry(self._selectedId)
            self.idLineEdit.setText(self._selectedId)
            self.englishTextEdit.setPlainText(entry["en"])
            self.germanTextEdit.setPlainText(entry["de"])
            self.idLineEdit.setEnabled(True)
            self.englishTextEdit.setEnabled(True)
            self.germanTextEdit.setEnabled(True)

        self.idList.clear()
        self.idList.addItems(self._strings.getIdList())

        self._uiLocked = False
        

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv) # Create an instance of QtWidgets.QApplication
    app.setWindowIcon(QtGui.QIcon(script_dir + '/text_editor_icon.png'))

    filename = None
    if len(sys.argv) > 1:
        filename = sys.argv[1]

    window = Ui(filename) # Create an instance of our class
    app.exec_() # Start the application
