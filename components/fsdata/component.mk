# Component makefile for fsdata

COMPONENT_ADD_LDFLAGS=-lstdc++ -l$(COMPONENT_NAME)

COMPONENT_ADD_INCLUDEDIRS := include

COMPONENT_SRCDIRS := src