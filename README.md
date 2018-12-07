# BatchProcessAssets


功能

可以批量缩小UE4纹理尺寸，仅用于编辑器下调试，加快纹理资源的载入速度，减少内存占用

提供32-512最大纹理选择

使用方法

1.将插件安装到工程的Plugins目录并启用它，选择引擎Modes区域下增加的工具栏BatchProcessAssetsTools，进入设置页面

2.点击ScaleTextures按钮开始对选择的纹理（或不选择任何纹理，则相当于选择所有纹理）进行缩放


关于DoNotModifyOrigenalAssets选项
默认不选中，表示会将原来的纹理大小修改到SelectMaxTextureSize纹理大小，当然如果原纹理本来就小于SelectMaxTextureSize则没有任何改变，第二次处理纹理时速度更快（如果SelectMaxTextureSize一样），因为不需要加载整个纹理到内存。

选中后表示仅会改变原纹理的最大纹理设置（Maximum Texture Size）而不会改变将其实际大小，二次处理速度稍慢，好处是随时可以还原


注意事项：

1.处理完成后只会将导入结果存入缓存，需要手动保存

2.保存前自行备份所有纹理数据