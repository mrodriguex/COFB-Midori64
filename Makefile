
AUTO : ./bin/cifrador 
OBJ_DIR = ./obj
INCL_DIR = -Ilib 

$(OBJ_DIR)/cifrador.o: app/cifrador.c lib/cofb.h
	$(CC) $(PROF) $(FLAGS_CC) -o $(OBJ_DIR)/cifrador.o $(INCL_DIR) -c app/cifrador.c 
	$(COMMANDS) 

$(OBJ_DIR)/misc.o: src/misc.c lib/misc.h
	$(CC) $(PROF) $(FLAGS_CC) -o $(OBJ_DIR)/misc.o $(INCL_DIR) -c src/misc.c 
	$(COMMANDS) 

$(OBJ_DIR)/midori.o: src/midori.c lib/midori.h
	$(CC) $(PROF) $(FLAGS_CC) -o $(OBJ_DIR)/midori.o $(INCL_DIR) -c src/midori.c 
	$(COMMANDS) 

$(OBJ_DIR)/cofb.o: src/cofb.c lib/cofb.h
	$(CC) $(PROF) $(FLAGS_CC) -o $(OBJ_DIR)/cofb.o $(INCL_DIR) -c src/cofb.c 
	$(COMMANDS) 

ALL_OBJ = $(OBJ_DIR)/cifrador.o $(OBJ_DIR)/misc.o $(OBJ_DIR)/midori.o $(OBJ_DIR)/cofb.o 

./bin/cifrador : $(ALL_OBJ)
	cc -mavx2 -maes -o ./bin/cifrador $(ALL_OBJ)
