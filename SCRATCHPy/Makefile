
TOOL = python3 setup.py

all:
	$(TOOL) build

run:
	LD_LIBRARY_PATH="../build-libSCRATCHy" python3 scratchpytest.py

repl:
	LD_LIBRARY_PATH="../build-libSCRATCHy" python3 -i scratchpytest.py

clean:
	$(TOOL) clean --all
