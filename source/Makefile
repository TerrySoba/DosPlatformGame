objects_game_lib = \
    vgagfx.o image.o animation.o keyboard.o rad_player.o \
    rad_player_asm.o timer.o physics.o tga_image.o \
    level.o blit.o detect_lines.o exception.o \
	rectangle.o tiny_string.o game.o compiled_sprite.o \
	joystick.o enemy.o game_save.o text.o i18n.o fire_ball.o \
	soundblaster.o sound_controller.o seeker_enemy.o \
	boss1.o time_tools.o music_controller.o key_mapper.o
objects_game = \
	main.o $(objects_game_lib)
objects_test = \
    $(objects_game_lib) test_main.o unit_test.o \
	physics_test.o tga_image_test.o shared_ptr_test.o \
	blit_test.o vector_test.o rectangle_test.o tiny_string_test.o animation_test.o \
	text_layout_test.o i18n_test.o level_test.o raii_ptr_test.o ptr_vector_test.o test_class.o \
	time_tools_test.o

rad_tester = \
	keyboard.o rad_tester.o timer.o rad_player.o rad_player_asm.o exception.o tiny_string.o
	
title_screen = \
    title.o vgagfx.o image.o tga_image.o exception.o rectangle.o keyboard.o \
	tiny_string.o animation.o compiled_sprite.o tiny_string.o


# coptions = -0 -ml -xs -onatx -oh -oi+ -ei -zp8 -fpi87
# coptions = -0 -ml -xs -onatx -oh -ol -ol+ -ox
coptions = -ml -oneatx -xs -xr -zp4 -0 -ad

coptions_launcher = -ms -oneatx -xd -zp4 -0 -ad -os

dest     = game2.exe
test     = gametest.exe
title    = title.exe
radtest = radtest.exe
launcher = game.exe


BUILD_DATE=`date -R`

all: $(dest) $(test) $(title) $(launcher) $(radtest)

$(dest): _$(dest)
	upx -9 --ultra-brute -f --8086 -o$(dest) _$(dest)

$(title): _$(title)
	upx -9 --ultra-brute -f --8086 -o$(title) _$(title)


$(radtest): 

%.o : %.cpp
	wpp $(coptions) $<

-include *.d


#launcher.o: launcher.cpp
#	wpp $(coptions_launcher) launcher.cpp
#	wdis launcher.o > launcher.asm

# soundblaster.o : soundblaster.cpp
# 	wpp $(coptions) soundblaster.cpp
# 	wdis soundblaster.o > soundblaster.asm

#compiled_sprite.o : compiled_sprite.cpp
#	wpp $(coptions) compiled_sprite.cpp
#	wdis compiled_sprite.o

#joystick.o : joystick.cpp
#	wpp $(coptions) joystick.cpp
#	wdis joystick.o

rad_player_asm.o: 3rd_party/rad_tracker/c_interface.asm 3rd_party/rad_tracker/player_nasm.asm
	nasm -f obj -i 3rd_party/rad_tracker  3rd_party/rad_tracker/c_interface.asm -o rad_player_asm.o -DRAD_NO_DEFAULT_PLAYER

FORCE:

main.o: version.h

version.h: FORCE
	echo \#define BUILD_DATE \"$(BUILD_DATE)\" > version.h

_$(dest): $(objects_game)
	echo "NAME   $@" > game.lnk
	echo "SYSTEM DOS" >> game.lnk
	echo "OPTION STACK=16000" >> game.lnk
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

$(launcher): launcher.o
	echo "NAME   $@" > launcher.lnk
	echo "SYSTEM DOS" >> launcher.lnk
	echo "FILE   {launcher.o}" >> launcher.lnk
	wlink  @launcher.lnk

$(radtest): $(rad_tester)
	echo "NAME   $@" > radtest.lnk
	echo "SYSTEM DOS" >> radtest.lnk
	echo "FILE   {$(rad_tester)}" >> radtest.lnk
	wlink  @radtest.lnk
