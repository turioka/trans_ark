# trans_ark

Windows APIやOpenGLで透明なウィンドウを使ってアルカノイドのようなゲームを作りました。

実行の様子です。

https://github.com/user-attachments/assets/cc00f55f-f949-43f8-ad6c-9cc5417110a1



## 参考にしたコード

* https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/wic/wicviewerd2d
* https://web.archive.org/web/20210318094247/https://www.oit.ac.jp/is/L231/pukiwiki/index.php?%E6%84%9F%E8%A6%9A%E3%83%A1%E3%83%87%E3%82%A3%E3%82%A2%E7%A0%94%E7%A9%B6%E5%AE%A4/OpenGL/%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3%E3%83%9E%E3%83%83%E3%83%94%E3%83%B3%E3%82%B0
* https://tokoik.github.io/GLFWdraft.pdf
* 公式リファレンス

## コンパイルの方法（一応）
### Windows
Visual Studioで、プロパティを適切にしてコンパイルする。
### mac
`gcc arukanoido.cpp -o out -I /path/to/glfw/include -L /path/to/glfw/lib/ -lglfw3 -framework OpenGL -framework Cocoa -framework IOkit -framework CoreVideo`
