# プログラム名とオブジェクトファイル名
program = main
objs = main.o common.o disassemble.o headerPaser.o textPaser.o instractionPaser.o 8086.o 8086Log.o

# 定義済マクロの再定義
CC = gcc
CFLAGS = -g -Wall

# サフィックスルール適用対象の拡張子の定義
.SUFFIXES: .c .o

# プライマリターゲット
$(program): $(objs)
	$(CC) -o $(program) $^

# サフィックスルール
.c.o:
	$(CC) $(CFLAGS) -c $<

run: main
	./main -d tests/a.out

# ファイル削除用ターゲット
.PHONY: clean
clean:
	$(RM) $(program) $(objs)