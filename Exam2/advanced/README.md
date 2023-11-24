# 使用OpenGL显示三维模型-进阶

## 代码明细

- ReadPic() 读取$bunny.obj$的数据，并将v（几何体顶点）、vt（贴图坐标点）、vn（顶点法线）、f（面片）
保存到相应数据结构中。
- getnorm() 计算vn（顶点法线）。
- Initscene() 初始化画布。
- drawface() 将三角面片渲染成三维模型。
- trim() 使用滚轮进行缩放。
- rotate() 使用方向键进行旋转。
- display() 清空屏幕，设定光源。
- main() 初始化窗口大小、窗口位置、底色等参数信息，然后画出图案。

## 环境配置

- OS：Ubuntu20.04
- gcc
- OpenGL/glut

## 运行命令

- 控制台输入命令 `sh main.sh`

## 实验效果
![](Exam2-advanced.png)