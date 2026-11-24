# Math24

[简体中文](#简体中文) · [English](#english)

## 简体中文

Math24 是一款 Android 算术益智应用，可以求解经典的 24 点题目，也可以用自定义目标值生成或求解四则运算题。每个表达式使用所有给定数字，每个数字使用一次。

### 安装

需要 Android 10（API 29）或更高版本，以及支持 `arm64-v8a` 的设备。发布 APK 后，可在本仓库的 **Releases** 页面下载并安装。

### 使用

- **求解（Solve）**：在 **Rank** 输入目标值，在 **Values** 输入数字，每两个数字之间用一个空格分隔。求解会自动识别数字个数，数字和目标值都可以用 `3/4` 这样的分数形式。
- **出题（Generate）**：在 **Rank** 输入目标值，在 **Number of Operands** 输入操作数个数，在 **Values** 依次输入两个整数作为生成范围参数，例如 `1 13`。点 **Generate** 生成题目，再点 **Answer** 查看参考答案。
- **求解进度**：运行时会显示进度。点 **Stop** 可停止求解。点结果区域可复制显示的内容。

例如，将 **Rank** 设为 `24`，**Values** 设为 `3 3 8 8`。一个解是 `8 / (3 - 8 / 3) = 24`。

### 从源码构建

1. 用 Android Studio 打开项目根目录，通过 SDK Manager 安装 Android SDK Platform 37、NDK (Side by side) 和 CMake 3.22.1。
2. 同步 Gradle，选择兼容的 Android 设备或模拟器，点击 **Run**。

也可以在项目根目录用 Gradle Wrapper 构建调试 APK。

```powershell
.\gradlew.bat :app:assembleDebug
```

```sh
./gradlew :app:assembleDebug
```

APK 输出到 `app/build/outputs/apk/debug/app-debug.apk`。

### 许可证

本项目采用 MIT 许可证。见 [LICENSE](LICENSE)。

## English

Math24 is an Android arithmetic puzzle app. Solve classic 24-game puzzles or generate and solve puzzles with a custom target using addition, subtraction, multiplication, and division. A solution uses every supplied number exactly once.

### Installation

Math24 requires Android 10 (API 29) or later and an `arm64-v8a` device. Once an APK is published, download and install it from this repository’s **Releases** page.

### Usage

- **Solve** — Enter the target in **Rank** and the numbers in **Values**, separated by single spaces. The solver counts the numbers automatically. Both the target and the numbers can be fractions such as `3/4`.
- **Generate** — Enter the target in **Rank** and the operand count in **Number of Operands**. In **Values**, enter two integers as the generation range parameters, such as `1 13`. Tap **Generate** to make a puzzle and **Answer** to reveal a solution.
- **Solver progress** — The app displays progress while solving. Tap **Stop** to cancel the search. Tap the result area to copy the displayed text.

For example, enter `24` in **Rank** and `3 3 8 8` in **Values**. One solution is `8 / (3 - 8 / 3) = 24`.

### Build from source

1. Open the project root in Android Studio. Use SDK Manager to install Android SDK Platform 37, NDK (Side by side), and CMake 3.22.1.
2. Sync Gradle, select a compatible Android device or emulator, and click **Run**.

You can also use the Gradle Wrapper to build a debug APK from the project root.

```powershell
.\gradlew.bat :app:assembleDebug
```

```sh
./gradlew :app:assembleDebug
```

The APK is written to `app/build/outputs/apk/debug/app-debug.apk`.

### License

Math24 is released under the MIT License. See [LICENSE](LICENSE).
