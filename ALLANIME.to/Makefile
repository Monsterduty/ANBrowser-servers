
FRAMEWOKS=${FRAMEWORKS}

INC= -I $(FRAMEWOKS)/Qtest/include/QtWidgets/ -I $(FRAMEWOKS)/Qtest/include -I $(FRAMEWOKS)/Qtest/include/QtGui -I $(FRAMEWOKS)/Qtest/include/QtNetwork -I $(FRAMEWOKS)/Qtest/include/QtCore
LIBS= -L $(FRAMEWOKS)/Qtest/lib -lQt6Widgets -lQt6Gui -lQt6Core -lQt6Network
TAGS= -DQT_NO_VERSION_TAGGING
TOOLS= $(FRAMEWOKS)/Qtest/libexec/
CC= g++
OBJ= main.o
PCH= 
GOAL= a.out

$(GOAL):  $(OBJ) $(PCH)
	$(CC) $(OBJ) $(LIBS)
#$(TAGS)

main.o: main.cpp
	$(CC) -c main.cpp $(INC)

clean:
	rm -f *.o moc_* $(PCH) $(GOAL)
