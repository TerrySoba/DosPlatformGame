objects_game_lib = \
    image.o animation.o rad_player.o \
    rad_player_asm.o physics.o tga_image.o \
    level.o blit.o detect_lines.o exception.o \
	rectangle.o tiny_string.o game.o  \
	enemy.o game_save.o text.o i18n.o fire_ball.o \
	seeker_enemy.o \
	boss1.o time_tools.o  key_mapper.o \
	tentacle.o tentacle_arm.o eye.o boss2.o font.o font_writer.o \
	credits_loader.o credits_scroller.o palette_tools.o \
	platform/dos/vgagfx.o \
	platform/dos/compiled_sprite.o \
	platform/dos/soundblaster.o \
	platform/dos/dos_timer.o \
	platform/dos/keyboard_dos.o \
	platform/dos/joystick_dos.o \
	platform/dos/sound_controller_dos.o \
	platform/dos/music_controller_dos.o \
	convert_to_tga.o

objects_game = \
	platform/dos/main.o $(objects_game_lib)
	
objects_test = \
    $(objects_game_lib) test_main.o unit_test.o exception_test.o \
	physics_test.o tga_image_test.o shared_ptr_test.o \
	blit_test.o vector_test.o rectangle_test.o tiny_string_test.o animation_test.o \
	text_layout_test.o i18n_test.o level_test.o raii_ptr_test.o ptr_vector_test.o test_class.o \
	time_tools_test.o compiled_sprite_test.o font_test.o font_writer_test.o credits_loader_test.o

dissolve_objs = \
	platform/dos/vgagfx.o palette_tools.o image.o tiny_string.o exception.o platform/dos/dissolve.o \
	platform/dos/dos_timer.o platform/dos/keyboard_dos.o tga_image.o \
	convert_to_tga.o

title_screen = \
    platform/dos/title.o platform/dos/vgagfx.o image.o tga_image.o exception.o rectangle.o platform/dos/keyboard_dos.o \
	tiny_string.o animation.o platform/dos/compiled_sprite.o font.o font_writer.o \
	blit.o i18n.o palette_tools.o convert_to_tga.o

BUILD_DATE=$(shell date -R)
ABS_SOURCE_DIR=$(realpath $(SRC_DIR))
TEST_DATA_DIR=..\\\\\\\\testdata\\\\\\\\\

# coptions = -0 -ml -xs -onatx -oh -oi+ -ei -zp8 -fpi87
# coptions = -0 -ml -xs -onatx -oh -ol -ol+ -ox
coptions = -ml -oneatx -xs -xr -zp4 -0 -ad \
		   -dPLATFORM_DOS \
           -dBUILD_DATE="\"$(BUILD_DATE)\"" \
		   -dTEST_DATA_DIR="\"$(TEST_DATA_DIR)\""

coptions_launcher = -ms -oneatx -xd -zp4 -0 -ad -os

dest     = game2.exe
test     = gametest.exe
title    = title.exe
launcher = game.exe
dissolve = dissolve.exe


all: $(dest) $(test) $(title) $(launcher) $(radtest) $(dissolve) $(credits)

$(dest): _$(dest)
	./upx_compress.sh -9 --ultra-brute -f --8086 -o$(dest) _$(dest)

$(title): _$(title)
	./upx_compress.sh -9 --ultra-brute -f --8086 -o$(title) _$(title)

$(dissolve): 

%.o : %.cpp
	@mkdir -p $(dir $@)
	wpp $(coptions) -fo=$@ $<

-include *.d


#launcher.o: launcher.cpp
#	wpp $(coptions_launcher) launcher.cpp
#	wdis launcher.o > launcher.asm

# soundblaster.o : soundblaster.cpp
# 	wpp $(coptions) soundblaster.cpp
# 	wdis soundblaster.o > soundblaster.asm

#compiled_sprite.o : compiled_sprite.cpp
#	wpp $(coptions) compiled_sprite.cpp
#	wdis compiled_sprite.o > compiled_sprite.asm

# compiled_sprite_test.o : compiled_sprite_test.cpp
# 	wpp $(coptions) compiled_sprite_test.cpp
# 	wdis compiled_sprite_test.o > compiled_sprite_test.asm

#joystick.o : joystick.cpp
#	wpp $(coptions) joystick.cpp
#	wdis joystick.o

rad_player_asm.o: 3rd_party/rad_tracker/c_interface.asm 3rd_party/rad_tracker/player_nasm.asm
	nasm -f obj -i 3rd_party/rad_tracker 3rd_party/rad_tracker/c_interface.asm -o rad_player_asm.o -DRAD_NO_DEFAULT_PLAYER

FORCE:

# platform/dos/main.o: version.h test_data.h

#version.h: FORCE
#	echo \#define BUILD_DATE \"$(BUILD_DATE)\" > version.h

#test_data.h: FORCE
#	echo \#define TEST_DATA_DIR \"..\\\\\\\\testdata\\\\\\\\\" > test_data.h

_$(dest): $(objects_game)
	echo "NAME   $@" > game.lnk
	echo "SYSTEM DOS" >> game.lnk
	echo "OPTION STACK=20000" >> game.lnk
	echo "FILE   {$(objects_game)}" >> game.lnk
	wlink  @game.lnk

$(test): $(objects_test) 
	echo "NAME   $@" > gametest.lnk
	echo "SYSTEM DOS" >> gametest.lnk
	echo "OPTION STACK=8192" >> gametest.lnk
	#echo "OPTION MAP=$(test).map" >> gametest.lnk
	echo "FILE   {$(objects_test)}" >> gametest.lnk
	wlink  @gametest.lnk

_$(title): $(title_screen)
	echo "NAME   $@" > title.lnk
	echo "SYSTEM DOS" >> title.lnk
	echo "OPTION STACK=16000" >> title.lnk
	echo "FILE   {$(title_screen)}" >> title.lnk
	wlink  @title.lnk

$(launcher): platform/dos/launcher.o
	echo "NAME   $@" > launcher.lnk
	echo "SYSTEM DOS" >> launcher.lnk
	echo "FILE   {platform/dos/launcher.o}" >> launcher.lnk
	wlink  @launcher.lnk

$(radtest): $(rad_tester)
	echo "NAME   $@" > radtest.lnk
	echo "SYSTEM DOS" >> radtest.lnk
	echo "FILE   {$(rad_tester)}" >> radtest.lnk
	wlink  @radtest.lnk

$(dissolve): $(dissolve_objs)
	echo "NAME   $@" > dissolve.lnk
	echo "SYSTEM DOS" >> dissolve.lnk
	echo "FILE   {$(dissolve_objs)}" >> dissolve.lnk
	wlink  @dissolve.lnk

clean:
	find . -name "*.d" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.lnk" -type f -delete
	find . -name "*.exe" -type f -delete
	find . -name "*.err" -type f -delete