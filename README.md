# PuertsMixinGenerator
将 unreal 的蓝图文件生成对应的 ts 脚本

## 功能介绍
添加此插件后，在 Unreal 的蓝图或者 UMG 蓝图工具栏出现一个 Mixin 按钮，点击按钮后：
- 自动收集当前蓝图的信息，生成对应的 Mixin 脚本
- 生成对应 Mixin 脚本的 import 信息到一个 PuertsImportList 文件中
注意，如果对应脚本已存在，则不会生成脚本，以免覆盖现有逻辑。

### Mixin 文件样式参考
```ts
// PuertsMixinGenerator_Auto_Generator_Flag
import * as UE from 'ue';
import { blueprint } from 'puerts';
const BP_TestUI_Class = UE.Class.Load('/Game/UI/GameStart/TestUI.TestUI_C');
const BP_TestUI = blueprint.tojs<typeof UE.Game.UI.GameStart.TestUI.TestUI_C>(BP_TestUI_Class);

interface TestUI extends UE.Game.UI.GameStart.TestUI.TestUI_C {}

class TestUI {
	constructor() {

	}

}
blueprint.mixin(BP_TestUI, TestUI);
```

## 一些问题
Mixin 模板目前是直接写在 C++ 中的，不便调整。如需定制，则最好将信息导出为 JSON 等格式，再使用脚本工具更灵活地生成。
此插件主要提供了一个蓝图编辑器的按钮，并收集当前打开蓝图信息的功能。

## 功能依赖
- Unreal Engine
- 腾讯的 Puerts 项目

## License

This project (PuertsMixinGenerator) is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Please note that this project includes third-party components:
- **Puerts**, which is licensed under the BSD 3-Clause License.
- **doT**, which is licensed under the MIT License.

Their respective license texts are included in the LICENSE file.
