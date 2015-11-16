CFLAGS= -std=c11 -Wall -Werror -Wextra -pedantic -Wno-deprecated  -Wstack-usage=1024 

TARGET=project
OBJS=project.o

.PHONY: clean debug profile

$(TARGET): $(OBJS)


debug: CFLAGS+=g
debug: $(TARGET)


clean:
	-$(RM) $(TARGET) $(OBJS)

profile: CFLAGS+=-pg
profile: LDFLAGS+=-pg
profile: $(TARGET)
hangMan.o: 
