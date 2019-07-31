# cross compileで使う際のプレフィックス
# raspberry piはx86でarm64アーキテクチャのため
ARMGNU?=aarch64-linux-gnu

COPS=-Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
# COPS gccのcコンパイル時のオプション
# -Wall : 全てのWarningを表示する
# -nostdlib : Cのstandard libraryを使わない => OSによって実行されてしまうため。
# -nostartfiles : 標準のstart up fileを使わない => 自身で実装するため。
# -ffreestanding : freestanding環境で実行 => standard libを使わない為。
# -Iinclude : headerファイルをincludeディレクトリから検索する。
# -mgeneral-regs-only: 通常のレジスタを使う。ARMのNEONレジスタを使うと複雑性が増すため。

ASMOPS=-Iinclude
# ASMOPS gccのアセンブリコンパイル時のオプション

# ビルド済みバイナリ
BUILD_DIR=build
# ソースコード
SRC_DIR=src

# デフォルトターゲットkernel8.imgを含んだ全てのターゲットへリダイレクト
all: kernel8.img

# コンパイルされた全てのカーネルイメージを削除
clean:
	rm -rf $(BUILD_DIR)/*.img

# Cのコンパイル
# $<と$@はランタイムに保管される
# $<: 最初の依存関係ファイル名 $@: target名
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

# アセンブリのコンパイル
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

# OBJ_FILES: アセンブリ、Cのビルドファイルを持つ配列
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

# DEP_FILES: ヘッダーファイルの依存関係定義
DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES) # 再コンパイルする際に読み込む必要がある。

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	# kernel8.elfのbuild 実行可能なイメージの基本レイアウトを定義する (linker scriptは次のセクションで述べる)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o kernel8.elf $(OBJ_FILES)
	# elfファイルはOSによって実行されるため、全ての実行可能なプログラムとデータセクションをelfファイルからkernel8.imgへ抽出する。
	# 8は64bitアーキテクチャのARMv8を表す。
	# config.txtでarm_control=0x200を指定するとcpuを64bit modeでブートできる。
	$(ARMGNU)-objcopy kernel8.elf -O binary kernel8.img
