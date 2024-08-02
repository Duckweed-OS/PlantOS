## 构建

**注意：在构建之前，你需要安装 nasm、gcc、g++、mtools、qemu、cmake**

首先，你必须克隆此储存库并进入项目目录：

```cmd
git clone https://github.com/min0911Y/Plant-OS.git
cd Plant-OS.git
```

之后分为两种构建方法

### 新的构建方法

你只需要运行 build.sh 即可完成编译

```sh
./build.sh
```

### 旧的构建方法

其次，转到 apps 文件夹：

```cmd
cd apps
```

然后，使用 `make` 编译应用程序：

```cmd
make
```

如果你没有看到错误消息，则转到 `loader` 文件夹，然后在 cmd 提示符中输入 `make`：

```cmd
cd ..
cd loader
make
```

如果你没有看到错误消息，则您可以运行以下命令进入 `kernel` 文件夹并构建内核：

```cmd
cd ..
cd kernel
make
```

或者您可以添加 `run` 以便在编译后启动调试：

```cmd
make run
```

您将看到 Powerint DOS 在 kernel/img 文件夹中分成四个图像。

**完成！您现在可以使用 qemu 或任何其他您喜欢的虚拟化软件尝试 Powerint DOS！**

## 启动

在 `kernel` 目录中：

```cmd
make full_run
```

您还可以使用 `make run` 或 `make img_run`，它们有所不同。

## Doom 游戏

如果您想运行 Doom，在构建后：

1. 您可以二进制连接 `kernel/img/doom1.img` 和 `kernel/img/doom2.img`。之后，在 `kernel` 目录下运行：

```cmd
qemu-system-i386 -net nic,model=pcnet -net user -serial stdio -device sb16 -device floppy -fda ./img/Powerint_DOS_386.img -drive id=disk,file=disk.img,if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -hdb <YOUR-DOOM-HARD-DISK-FILE-NAME> -boot a -m 512 -enable-kvm
```

2. 你也可以使用 PlantOS 提供的 `doomcpy`，参见 [doomcpy.c](apps/doomcpy/doomcpy.c)。
