# this makefile is for x64 linux systems!
# adjust for your own system if needed
# -- MDVz0r

#CFLAGS = -O -g -DNDEBUG -DLINUX -Dstricmp=Q_stricmp -fPIC
CFLAGS = -ffast-math -O3 -w -DGAME_INCLUDE -DLINUX -fPIC
LDFLAGS = -S
ORIGDIR=src

OBJS = g_main.o

gamei386.so: $(OBJS)
	ld -lcurl -lm -shared -o $@ $(OBJS) $(LDFLAGS)
	chmod 0755 $@ 
	ldd $@

clean: 
	/bin/rm -f $(OBJS) gamex86_64.so

$*.o: $*.c
	$(CC) $(CFLAGS) -c $*.c

$*.c: $(ORIGDIR)/$*.c
	tr -d '\015' < $(ORIGDIR)/$*.c > $*.c

$*.h: $(ORIGDIR)/$*.h
	tr -d '\015' < $(ORIGDIR)/$*.h > $*.h

# DO NOT DELETE

fopen.o: g_local.h q_shared.h game.h
g_main.o: g_local.h q_shared.h game.h
md4.o: g_local.h q_shared.h game.h
regex.o: g_local.h q_shared.h game.h
zb_ban.o: g_local.h q_shared.h game.h
zb_acexcp.o: g_local.h q_shared.h game.h
zb_hashl.o: g_local.h q_shared.h game.h
zb_checkvar.o: g_local.h q_shared.h game.h
zb_clib.o: g_local.h q_shared.h game.h
zb_cmd.o: g_local.h q_shared.h game.h
zb_disable.o: g_local.h q_shared.h game.h
zb_flood.o: g_local.h q_shared.h game.h
zb_init.o: g_local.h q_shared.h game.h
zb_log.o: g_local.h q_shared.h game.h
zb_lrcon.o: g_local.h q_shared.h game.h
zb_msgqueue.o: g_local.h q_shared.h game.h
zb_spawn.o: g_local.h q_shared.h game.h
zb_util.o: g_local.h q_shared.h game.h
zb_vote.o: g_local.h q_shared.h game.h
zb_zbot.o: g_local.h q_shared.h game.h
zb_zbotcheck.o: g_local.h q_shared.h game.h
