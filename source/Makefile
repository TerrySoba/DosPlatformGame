objects_game_lib = \
    vgagfx.o image.o animation.o keyboard.o rad_player.o \
    rad_player_asm.o timer.o physics.o tga_image.o \
    json.o cJSON.o level.o blit.o detect_lines.o exception.o \
	rectangle.o tiny_string.o game.o compiled_sprite.o \
	joystick.o enemy.o game_save.o text.o i18n.o
objects_game = \
	main.o $(objects_game_lib)
objects_test = \
    $(objects_game_lib) test_main.o unit_test.o \
	physics_test.o csv_reader_test.o tga_image_test.o json_test.o shared_ptr_test.o \
	blit_test.o vector_test.o rectangle_test.o tiny_string_test.o animation_test.o \
	text_layout_test.o i18n_test.o
# coptions = -0 -ml -xs -onatx -oh -oi+ -ei -zp8 -fpi87
# coptions = -0 -ml -xs -onatx -oh -ol -ol+ -ox
coptions = -ml -oneatx -xs -zp4 -0

dest     = game.exe
test     = gametest.exe

BUILD_DATE=`date -R`

all: $(dest) $(test)

$(dest): _$(dest)
	upx -9 --ultra-brute -f --8086 -o$(dest) _$(dest)

main.o: vgagfx.h image.h animation.h keyboard.h rad_player.h version.h physics.h json.h level.h blit.h joystick.h
vgagfx.o: image_base.h animation.h rectangle.h font_data.h drawable.h
image.o: image_base.h
animation.o: image.h drawable.h image_base.h
keyboard.o:
rad_player.o: timer.h
timer.o: 
physics.o: rectangle.h
test_main.o: unit_test.h
unit_test.o: unit_test.h
physics_test.o: unit_test.h physics.h
csv_reader_test.o: unit_test.h csv_reader.h 
csv_reader.o: csv_reader.h
tga_image.o: tga_image.h
tga_image_test.o: tga_image.h unit_test.h
json.o: json.h
json_test.o: json.h
shared_ptr_test.o: shared_ptr.h
level.o: level.h image_base.h csv_reader.h drawable.h blit.h detect_lines.h
blit.o: rectangle.h image_base.h
detect_lines.o: detect_lines.h rectangle.h csv_reader.h
exception.o: exception.h
vector_test.o: vector.h unit_test.h
rectangle.o: rectangle.h 
tiny_string.o: tiny_string.h
rectangle_test.o: rectangle.h unit_test.h
game.o: game.h shared_ptr.h level.h animation.h physics.h vgagfx.h keyboard.h actor_animation_controller.h compiled_sprite.h
tiny_string_test.o: tiny_string.h
animation_test.o: animation.h unit_test.h
compiled_sprite.o: compiled_sprite.h drawable.h
joystick.o: joystick.h
enemy.o: enemy.h
game_save.o: game_save.h physics.h game.h
text_layout_test.o: text_layout.h unit_test.h
text.o: text.h drawable.h font_data.h
i18n.o: i18n.h
i18n_test.o: i18n.h


cJSON.o: 3rd_party/cJSON/cJSON.h
	wpp $(coptions) 3rd_party/cJSON/cJSON.c

%.o : %.cpp
	wpp $(coptions) $<

#compiled_sprite.o : compiled_sprite.cpp
#	wpp $(coptions) compiled_sprite.cpp
#	wdis compiled_sprite.o

#joystick.o : joystick.cpp
#	wpp $(coptions) joystick.cpp
#	wdis joystick.o

rad_player_asm.o: 3rd_party/rad_tracker/c_interface.asm 3rd_party/rad_tracker/player_nasm.asm
	nasm -f obj -i 3rd_party/rad_tracker  3rd_party/rad_tracker/c_interface.asm -o rad_player_asm.o -DRAD_NO_DEFAULT_PLAYER

FORCE:

version.h: FORCE
	echo \#define BUILD_DATE \"$(BUILD_DATE)\" > version.h

_$(dest): $(objects_game)
	echo "NAME   $@" > game.lnk
	echo "SYSTEM DOS" >> game.lnk
	echo "FILE   {$(objects_game)}" >> game.lnk
	wlink  @game.lnk

$(test): $(objects_test)
	echo "NAME   $@" > gametest.lnk
	echo "SYSTEM DOS" >> gametest.lnk
	echo "FILE   {$(objects_test)}" >> gametest.lnk
	wlink  @gametest.lnk