
TOOL = python3 setup.py

all:
	$(TOOL) build

run:
	LD_LIBRARY_PATH="../build-libSCRATCHy" python3 scratchPyTest.py

repl:
	LD_LIBRARY_PATH="../build-libSCRATCHy" python3 -i scratchPyTest.py

clean:
	$(TOOL) clean --all
