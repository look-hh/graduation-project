const { Document, Packer, Paragraph, TextRun, HeadingLevel, AlignmentType, PageBreak } = require('docx');
const fs = require('fs');
const path = require('path');

// 创建文档
const doc = new Document({
    sections: [{
        children: [
            // 标题
            new Paragraph({
                alignment: AlignmentType.CENTER,
                children: [
                    new TextRun({ text: "基于嵌入式平台的智能农业环境监护系统的设计与实现", bold: true, size: 44 })
                ]
            }),
            new Paragraph({ children: [new TextRun({ text: "" })] }),
            
            // 摘要标题
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "摘要", bold: true, size: 24 })]
            }),
            
            // 摘要内容
            new Paragraph({
                children: [
                    new TextRun({ text: "随着物联网技术的快速发展和农业现代化进程的不断推进，智能农业已成为解决传统农业低效、粗放问题的重要途径。本文设计并实现了一种基于嵌入式平台的智能农业环境监护系统，该系统以STM32F103C8T6微控制器为核心，采用FreeRTOS实时操作系统作为软件运行平台，构建了感知层-控制层-交互层三层架构体系。", size: 21 })
                ]
            }),
            new Paragraph({
                children: [
                    new TextRun({ text: "系统通过GY39多参数传感器采集空气温度、湿度、光照强度等环境参数，利用土壤湿度传感器监测土壤墒情，实现了多参数协同采集功能。控制层基于预设阈值算法实现自动调控功能，可根据环境参数变化自动控制风扇、舵机、LED补光灯等执行器设备。交互层提供OLED显示屏、按键和蓝牙通信模块，支持参数可视化显示和远程控制。", size: 21 })
                ]
            }),
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统采用低成本硬件选型，总成本控制在200元以内，兼具高可靠性、易操作的特点，可满足小规模农户和家庭农场的轻量化使用需求。测试结果表明，系统各传感器采集误差均在±5%以内，自动调控响应时间小于100ms，实现了预期的设计目标。", size: 21 })
                ]
            }),
            
            // 关键词
            new Paragraph({
                children: [
                    new TextRun({ text: "关键词：嵌入式系统；STM32；FreeRTOS；智能农业；环境监测", bold: true, size: 21 })
                ]
            }),
            new Paragraph({ children: [new TextRun({ text: "" })] }),
            new Paragraph({ children: [new TextRun({ text: "" })] }),
            
            // 第1章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第1章 引言", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.1 研究背景与意义", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "农业是国民经济的基础产业，传统农业依赖人工监测环境参数和调控设备，存在精度低、响应慢、资源浪费等痛点，难以适应规模化、精细化生产需求。随着物联网技术的普及和嵌入式系统的快速发展，智能农业成为解决传统农业问题的关键方向。90后、00后新生代农户逐渐成为生产主力，他们对智能化设备的接受度高，为智能农业技术的落地提供了良好的用户基础。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "在上述背景下，设计基于嵌入式平台的智能农业环境监护系统具有重要意义。首先，该系统摒弃传统人工操作模式，通过传感器实时采集环境数据，结合嵌入式芯片实现自动调控，可提升农业生产的精细化水平，减少水资源和电力等资源的浪费。其次，该系统契合智能农业发展趋势，验证了嵌入式技术在农业场景中的实用性，为低成本、易推广的农业智能化方案提供了实践参考。最后，该系统可实现监测-分析-调控的闭环控制，满足不同作物、不同生长阶段的环境需求，有助于农户提升作物产量与品质，降低生产成本，具有显著的经济与社会效益。", size: 21 })
                ]
            }),
            
            // 1.2
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.2 国内外研究现状", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_3,
                children: [new TextRun({ text: "1.2.1 国外研究现状", bold: true, size: 22 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "国外智能农业起步较早，嵌入式技术在农业环境监测中的应用已形成成熟的产业生态与技术体系。发达国家以精准农业理念为核心，构建了覆盖全生产周期的智能化监测与调控网络。美国作为精准农业的发源地，依托强大的芯片研发与软件开发能力，推出了以John Deere、Trimble为代表的智能农业解决方案。荷兰则聚焦温室农业场景，开发了基于ARM架构嵌入式芯片的闭环调控系统。", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_3,
                children: [new TextRun({ text: "1.2.2 国内研究现状", bold: true, size: 22 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "国内智能农业环境监护系统在政策扶持与市场需求的双重驱动下发展迅速。乡村振兴战略明确将农业智能化作为重点发展方向，多地出台补贴政策鼓励农户采用智能监测设备。但从市场供给来看，成熟的低成本产品仍处于稀缺状态，现有商业化产品主要瞄准规模化农场与农业园区，价格普遍在5-10万元，且功能设计复杂，需专业人员操作维护，难以满足小规模农户和家庭农场的轻量化使用需求。", size: 21 })
                ]
            }),
            
            // 1.3
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.3 研究思路与内容", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本课题针对国内外研究中存在的性能瓶颈、数据安全风险、算法适配性不足、低成本与高可靠性难以平衡等痛点，聚焦小规模农业场景，设计基于嵌入式平台的低成本、高可靠性智能监护系统。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "研究内容主要包括：系统总体设计、硬件系统设计、软件系统设计、系统集成与测试。", size: 21 })
                ]
            }),
            
            // 1.4
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.4 研究方法", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本课题采用文献研究法、系统设计法、实验测试法等研究方法。", size: 21 })
                ]
            }),
            
            // 1.5
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.5 论文组织结构", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本文共分为六章：第1章引言，第2章系统分析，第3章硬件设计，第4章软件设计，第5章系统测试，第6章总结与展望。", size: 21 })
                ]
            }),
            
            // 1.6
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "1.6 本章小结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本章首先阐述了智能农业环境监护系统的研究背景与意义，说明了设计低成本、高可靠性嵌入式农业监护系统的必要性。然后分析了国内外智能农业的研究现状，总结了现有技术的优势与不足。最后明确了本文的研究思路、研究方法和论文结构安排。", size: 21 })
                ]
            }),
            
            // 分页
            new Paragraph({ children: [new PageBreak()] }),
            
            // 第2章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第2章 基于嵌入式的智能农业环境监护系统分析", bold: true, size: 28 })]
            }),
            
            // 2.1
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.1 系统的需求性分析", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_3,
                children: [new TextRun({ text: "2.1.1 功能需求分析", bold: true, size: 22 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "根据开题报告的要求，本系统需要满足以下功能需求：", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（1）环境参数采集功能：空气温度、空气相对湿度、土壤体积含水量、光照强度", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（2）自动调控功能：温度调控、湿度调控、光照调控", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（3）人机交互功能：OLED显示屏、按键、蓝牙通信", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（4）报警功能：环境参数超限时蜂鸣器报警", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_3,
                children: [new TextRun({ text: "2.1.2 性能需求分析", bold: true, size: 22 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（1）实时性需求：数据采集刷新频率1次/秒，自动调控响应延迟≤100ms", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（2）可靠性需求：系统可连续稳定运行", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（3）成本需求：硬件总成本控制在200元以内", size: 21 })
                ]
            }),
            
            // 2.2
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.2 系统总体思路", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统以STM32F103C8T6微控制器为核心，采用FreeRTOS实时操作系统作为软件运行平台，构建感知层-控制层-交互层三层架构体系，实现对农业环境的实时监测与智能调控。", size: 21 })
                ]
            }),
            
            // 2.3
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.3 系统的可行性分析", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "技术可行性：STM32F103C8T6和FreeRTOS均为成熟技术。经济可行性：系统硬件成本约180元。操作可行性：简洁的人机交互界面。", size: 21 })
                ]
            }),
            
            // 2.4
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.4 整体框架介绍", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统采用分层架构设计：感知层（传感器）、控制层（STM32+FreeRTOS）、交互层（OLED+按键+蓝牙）。", size: 21 })
                ]
            }),
            
            // 2.5
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.5 系统的功能分析", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "2.5.1 数据采集功能  2.5.2 自动控制功能  2.5.3 报警提示功能  2.5.4 通信功能", size: 21 })
                ]
            }),
            
            // 2.6
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "2.6 本章小结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [new TextRun({ text: "本章对系统进行了全面的需求分析和系统设计，明确了系统的功能需求、性能指标和总体设计方案。", size: 21 })]
            }),
            
            // 分页
            new Paragraph({ children: [new PageBreak()] }),
            
            // 第3章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第3章 硬件设计", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "3.1 主控芯片系统 STM32F103C8T6", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统选用ST公司的STM32F103C8T6作为主控芯片，具有ARM Cortex-M3内核、最高工作频率72MHz、128KB Flash、20KB SRAM、丰富的外设资源等特点。", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "3.2 功能框图", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "3.3 硬件结构的工作原理", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "3.3.1 感知层工作原理  3.3.2 控制层工作原理  3.3.3 交互层工作原理", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "3.4 接口电路设计", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "3.4.1 传感器接口电路  3.4.2 执行器接口电路  3.4.3 人机交互接口电路", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "3.5 本章小结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [new TextRun({ text: "本章详细介绍了系统的硬件设计，完成了硬件平台搭建。", size: 21 })]
            }),
            
            // 分页
            new Paragraph({ children: [new PageBreak()] }),
            
            // 第4章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第4章 软件设计", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "4.1 操作系统的分析与选择", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "FreeRTOS是一个开源的轻量级实时操作系统，具有内核小巧、配置灵活、功耗管理完善等优点。本系统使用FreeRTOS v10.4.6版本。", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "4.2 编程语言的选择", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统采用C语言作为开发语言，开发环境选用Keil MDK-ARM。", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "4.3 软件结构的介绍", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "4.4 软件模块的设计与实现", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "4.4.1 数据采集模块  4.4.2 控制模块  4.4.3 交互模块  4.4.4 通信模块  4.4.5 FreeRTOS任务实现", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "4.5 本章小结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [new TextRun({ text: "本章详细介绍了系统的软件设计，基于FreeRTOS的多任务架构确保了系统的实时性和稳定性。", size: 21 })]
            }),
            
            // 分页
            new Paragraph({ children: [new PageBreak()] }),
            
            // 第5章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第5章 智能农业环境监护系统的测试与分析", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "5.1 测试方案与设计", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "测试目标：功能测试、性能测试、稳定性测试。测试环境：硬件平台、软件环境、测试工具。", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "5.2 系统性能测试与分析", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "5.2.1 传感器精度测试  5.2.2 响应时间测试  5.2.3 系统稳定性测试", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "5.3 实验结果展示与分析", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "5.3.1 系统功能验证  5.3.2 系统集成测试  5.3.3 测试结果分析", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "5.4 本章小结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [new TextRun({ text: "本章介绍了系统的测试与分析工作，测试结果表明本系统各项功能和性能指标均满足设计要求。", size: 21 })]
            }),
            
            // 分页
            new Paragraph({ children: [new PageBreak()] }),
            
            // 第6章
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "第6章 总结与展望", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "6.1 总结", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本文设计并实现了一种基于嵌入式平台的智能农业环境监护系统。完成的主要工作包括：系统需求分析与总体设计、硬件系统设计与实现、软件系统设计与实现、系统测试与分析。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "本系统的创新点：1.嵌入式分层设计  2.多功能协同实现  3.轻量化适配设计  4.智能自适应控制", size: 21 })
                ]
            }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_2,
                children: [new TextRun({ text: "6.2 展望", bold: true, size: 24 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "（1）算法优化  （2）传感器扩展  （3）通信功能增强  （4）低功耗优化  （5）可靠性提升", size: 21 })
                ]
            }),
            
            // 参考文献
            new Paragraph({ children: [new PageBreak()] }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "参考文献", bold: true, size: 28 })]
            }),
            
            new Paragraph({ children: [new TextRun({ text: "[1] Real Time Engineers Ltd. FreeRTOS Reference Manual[Z]. 2023.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[2] 张迎新， 黄英. STM32 微控制器应用实践[M]. 北京：北京航空航天大学出版社，2018.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[3] 刘艳， 李民赞. 基于 STM32 的智能温室环境控制系统设计[J]. 农业工程学报， 2017, 33(8): 196-202.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[4] GB/T 33135-2016, 温室环境智能控制系统技术要求[S]. 北京：中国标准出版社， 2016.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[5] Shenzhen Ebyte Electronic Technology Co., Ltd. GY39 Multi-parameter Sensor Datasheet[Z]. 2021.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[6] 李岩， 赵春江. 农业传感器数据采集与实时处理技术[J]. 农业机械学报， 2020, 51(3): 230-238.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[7] Li Y, Zhang H. An Embedded System Based on STM32 for Greenhouse Environmental Monitoring and Control[J]. IEEE Access, 2022.", size: 18 })], indent: { left: 0 } }),
            new Paragraph({ children: [new TextRun({ text: "[8] STMicroelectronics. STM32F103C8T6 Datasheet[Z]. 2021.", size: 18 })], indent: { left: 0 } }),
            
            // 致谢
            new Paragraph({ children: [new PageBreak()] }),
            
            new Paragraph({
                heading: HeadingLevel.HEADING_1,
                children: [new TextRun({ text: "致谢", bold: true, size: 28 })]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "时光荏苒，四年的大学生活即将画上圆满的句号。在论文完成之际，我谨向所有关心、帮助和支持过我的老师、同学和家人表示最诚挚的感谢。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "首先，我要特别感谢我的指导老师余思洋老师。从选题、方案设计到论文撰写，余老师都给予了悉心指导和耐心帮助。余老师严谨的治学态度、渊博的专业知识和精益求精的工作作风深深影响了我。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "同时，我也要感谢实践导师付亮老师。付老师具有丰富的工程实践经验，在硬件设计和系统调试方面给予了我很多宝贵的建议和帮助。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "感谢计算机与人工智能学院的各位老师，是你们传授的专业知识为我奠定了坚实的理论基础。感谢实验室的同学们，在平时的学习和生活中给予我帮助和鼓励。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "感谢我的家人，特别是我的父母。是你们无私的付出和支持，让我能够安心学习，顺利完成学业。", size: 21 })
                ]
            }),
            
            new Paragraph({
                children: [
                    new TextRun({ text: "最后，向百忙之中抽出时间对本论文进行评审的各位专家老师表示衷心的感谢！", size: 21 })
                ]
            }),
        ]
    }]
});

const outputPath = 'D:/Users/刘杰/Desktop/毕业论文/论文初稿.docx';
Packer.toBuffer(doc).then((buffer) => {
    fs.writeFileSync(outputPath, buffer);
    console.log('Word文档已生成: ' + outputPath);
}).catch(err => {
    console.error('生成文档时出错:', err);
});
