# Squake
A multi-source Squirrel compiler

## Purpose

This app is designed to make compiling Squirrel source files easier.

## Use

After building the app, simply navigate to a folder with a squakefile and nuts, and call the app from there. It will read the squakefile and attempt to compile the nuts.

The squakefile should have each file on a separate line. The first line is the name of the output file, for example: `myapp.sq`. Each line after that is a source file. They should be listed in order of dependancy, meaning that if one depends on something defined in another, it should be listed after the one it depends on.

## To Do

Make a proper makefile for the compiler itself, since I don't know how to do this.
