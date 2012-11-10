CC= gcc
AVRO = src
JANSSON = jansson/src
GM_INC = ../inc
OBJ = obj
CFLAGS = -O3 -g -I$(AVRO) -I$(JANSSON) -I$(GM_INC) -Wall 

AVRO_SOURCES := $(wildcard $(AVRO)/*.c)
JANSSON_SOURCES := $(wildcard $(JANSSON)/*.c) 
AVRO_OBJECTS := $(subst ,,$(AVRO_SOURCES:.c=_avro.o)) 
AVRO_OBJECTS := $(filter-out $(AVRO)/avroappend_avro.o $(AVRO)/avropipe_avro.o $(AVRO)/avrocat_avro.o $(AVRO)/avromod_avro.o $(AVRO)/datum_equal_avro.o $(AVRO)/datum_size_avro.o  $(AVRO)/schema_specific_avro.o,$(AVRO_OBJECTS))
JANSSON_OBJECTS := $(subst ,,$(JANSSON_SOURCES:.c=_json.o)) 

include ../../common.mk

all: $(AVRO_OBJECTS) $(JANSSON_OBJECTS)
#	$(CC) $(CFLAGS) $^ $(LINK_AVRO_LIB) -o $@

$(JANSSON)/%_json.o: $(JANSSON)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(AVRO)/%_avro.o: $(AVRO)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(AVRO)/*.o
	rm -f $(JANSSON)/*.o
