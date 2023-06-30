PKG_CONFIG_PATH="/usr/local/opt/openblas/lib/pkgconfig"

# CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags openblas)
# LDFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs openblas)

CFLAGS += -IC:/Users/BittlesNet/git/mingl/build -IC:/Users/BittlesNet/git/mingl/build/generated
LDFLAGS += -LC:/Users/BittlesNet/git/mingl/build/third_party/OpenBLAS/lib -lopenblas

LDFLAGS += -lm

.PHONY: all
all: quickblas

.PHONY: debug
debug:
	@echo $(CFLAGS)

quickblas: main.c
	clang -O3 -Wall -Wextra -o quickblas main.c $(CFLAGS) $(LDFLAGS)
