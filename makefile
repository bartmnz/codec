CFLAGS= -std=c11 -Wall -Werror -Wextra -pedantic -Wno-deprecated  -Wstack-usage=1024 

TARGET1=project
TARGET2=encoder
OBJS1=project.o meditrik.o 
OBJS2=encoder.o meditrik.o
.PHONY: clean debug profile

all: $(TARGET1) $(TARGET2) 
$(TARGET1): $(OBJS1)
$(TARGET2): $(OBJS2)


debug: CFLAGS+=-g
debug: all


clean:
	-$(RM) $(TARGET1) $(OBJS1) $(TARGET2) $(OBJS2)

profile: CFLAGS+=-pg
profile: LDFLAGS+=-pg
profile: $(TARGET)
 
