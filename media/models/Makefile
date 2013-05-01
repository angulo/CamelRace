MODELS := $(patsubst $(DIR_MODELS)%.mesh.xml,$(DIR_MODELS)%.mesh,$(wildcard $(DIR_MODELS)*.mesh.xml))

all: meshes

meshes: $(MODELS)
	@echo "Done!"

%.mesh:
	@echo "Generating $@..."
	@OgreXMLConverter -q $@.xml 

clean:
	@echo "Cleaning...";
	@rm -rf $(MODELS) *.log
	@echo "Done!"
