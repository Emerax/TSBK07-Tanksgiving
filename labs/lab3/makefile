# set this variable to the director in which you saved the common files
commondir = ../common/

all : lab3-1 lab3-3 lab3-4 lab3-5

lab3-1 : lab3-1.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab3-1 -I$(commondir) -I../common/Mac -DGL_GLEXT_PROTOTYPES lab3-1.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -framework OpenGL -framework Cocoa -lm 

lab3-3 : lab3-3.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab3-3 -I$(commondir) -I../common/Mac -DGL_GLEXT_PROTOTYPES lab3-3.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -framework OpenGL -framework Cocoa -lm 

lab3-4 : lab3-4.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab3-4 -I$(commondir) -I../common/Mac -DGL_GLEXT_PROTOTYPES lab3-4.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -framework OpenGL -framework Cocoa -lm 

lab3-5 : lab3-5.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o lab3-5 -I$(commondir) -I../common/Mac -DGL_GLEXT_PROTOTYPES lab3-5.c $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -framework OpenGL -framework Cocoa -lm 


clean :
	rm lab3-1
	rm lab3-3
	rm lab3-4
	rm lab3-5
