LEVELS=$(shell ls *.tmx | sed 's/tmx/map/')

all: $(LEVELS)

%.map : %.tmx
	python3 ../MapTool/map_tool.py $<

clean:
	rm -f $(LEVELS)
