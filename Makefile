EXEC = raytracing

IMGS := \
	image.ppm \
	check.png \
	result.png 

.PHONY: all
all: $(EXEC)

CC ?= gcc
CFLAGS = \
	-std=gnu99 -Wall -O3 -g -mavx2 -mfma -fopenmp
LDFLAGS = \
	-O3 \
	-lm \
	-fopenmp \
	-mavx2 \
	-mfma

ifeq ($(strip $(PROFILE)),1)
PROF_FLAGS = -pg
CFLAGS += $(PROF_FLAGS)
LDFLAGS += $(PROF_FLAGS) 
endif

OBJS := \
	object_sphere.o \
	raytracing.o 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

check: $(EXEC)
	@./$(EXEC) && convert image.ppm check.png || (echo Fail; exit)
	@echo "Verified OK"

.PHONY: clean
clean:
	$(RM) $(EXEC) $(OBJS) $(IMGS)