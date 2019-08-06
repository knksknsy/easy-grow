#
# Easy Grow Makefile
#

PROJECT_NAME := easy_grow

include $(IDF_PATH)/make/project.mk


SOURCE_DOCS := $(wildcard *.md)
INCLUDE_DIR := ../main/include


EXPORTED_DOCS=\
 $(SOURCE_DOCS:.md=.html) \

RM=/bin/rm

PANDOC=pandoc

PANDOC_OPTIONS= --standalone --metadata pagetitle="Documentation"

PANDOC_HTML_OPTIONS=--to html5


%.html : %.md
	$(PANDOC) $(PANDOC_OPTIONS) $(PANDOC_HTML_OPTIONS) -o $@ $<
	sed -i 's:\.md:.html:g' $@

.PHONY: all clean

documentation : $(EXPORTED_DOCS)
				$(EXPORTED_DOCS): $(INCLUDE_DIR)/$$@
				
	

clean_documentation:
	- $(RM) $(EXPORTED_DOCS)








