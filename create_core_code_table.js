const { Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell,
        AlignmentType, BorderStyle, WidthType, ShadingType, HeadingLevel,
        PageOrientation } = require('docx');
const fs = require('fs');

// ==================== 三线表样式定义 ====================
const topBorder = { style: BorderStyle.SINGLE, size: 12, color: "000000" };   // 顶线 1.5pt
const headerBorder = { style: BorderStyle.SINGLE, size: 6, color: "000000" };  // 栏目线 0.75pt
const bottomBorder = { style: BorderStyle.SINGLE, size: 12, color: "000000" }; // 底线 1.5pt
const noBorder = { style: BorderStyle.NONE };

// 只有顶线的单元格边框（表头行）
const headerBorders = {
    top: topBorder,
    bottom: headerBorder,
    left: noBorder,
    right: noBorder,
};
// 只有底线的单元格边框（最后一行）
const lastBorders = {
    top: noBorder,
    bottom: bottomBorder,
    left: noBorder,
    right: noBorder,
};
// 无边框（中间行）
const midBorders = {
    top: noBorder,
    bottom: noBorder,
    left: noBorder,
    right: noBorder,
};

const cellMargins = { top: 60, bottom: 60, left: 100, right: 100 };

// ==================== 工具函数：创建代码单元格 ====================
function codeCell(text, width, isHeader = false, isLast = false) {
    const borders = isHeader ? headerBorders : (isLast ? lastBorders : midBorders);
    return new TableCell({
        borders,
        width: { size: width, type: WidthType.DXA },
        margins: cellMargins,
        shading: isHeader ? { fill: "F2F2F2", type: ShadingType.CLEAR } : undefined,
        verticalAlign: "top",
        children: text.split('\n').map(line =>
            new Paragraph({
                spacing: { before: 20, after: 20, line: 276 }, // 行间距
                children: [
                    new TextRun({
                        text: line || " ",
                        font: isHeader ? "黑体" : "Consolas",
                        size: isHeader ? 22 : 18, // 五号 10.5pt=21 half-pt
                        bold: isHeader,
                    }),
                ],
            })
        ),
    });
}

// ==================== 核心代码内容（仅3部分）====================
const codeData = [
    {
        id: "1",
        module: "主程序\nFreeRTOS\n任务架构\n(main.c)",
        code: `int main(void) {\n  OLED_Init(); ... GY39_init();    // 外设初始化\n  bluetooth_init();\n  xTask3_Sem = xSemaphoreCreateBinary();  // 信号量\n\n  // Task1 自动控制: prio=3, stack=256, 周期2s\n  xTaskCreate(myTask1, "Task1", 256, NULL, 3, &h1);\n  // Task2 交互处理: prio=2, stack=256, 周期100ms\n  xTaskCreate(myTask2, "Task2", 256, NULL, 2, &h2);\n  // Task3 传感器: prio=4, stack=512, 信号量驱动\n  xTaskCreate(myTask3, "Task3", 512, NULL, 4, &h3);\n\n  vTaskStartScheduler();            // 启动调度\n}`,
    },
    {
        id: "2",
        module: "PID控制\n算法\n(show.c)",
        code: `#define FAN_KP  200.0f   // 比例系数\n#define FAN_KI   10.0f   // 积分系数\n#define FAN_KD   50.0f   // 微分系数\n#define FAN_PID_T 2.0f   // 采样周期2s\nvoid Fan_PID_ctrl(uint32_t set_wd) {\n  error = pv - setpoint;             // 偏差 e(k)\n  if (|error| < 0.3) { 关风扇; return; } // ±0.3℃死区\n\n  p_out = KP * error;                // 比例项\n  integral += error * T;             // 积分累加\n  integral = clamp(integral, 0, 100);// 抗饱和限幅\n  i_out = KI * integral;             // 积分项\n  d_out = KD * (error−prev) / T;     // 微分项\n  prev_error = error;\n\n  output = p_out + i_out + d_out;    // u(k)=P+I+D\n  output = clamp(output, 0, 10000);  // 输出限幅\n  if (output>0 && output<3000) output=3000; // 最小PWM\n  Fan_SetSpeed((uint16_t)output);    // 驱动风扇\n}`,
    },
    {
        id: "3",
        module: "传感器\n数据采集\n(GY39.c +\nadc.c)",
        code: `// GY39: UART3, 9600bps, 双模式交替采集\nvoid get_gy39_data(void) {\n  if (mode == GUANGZHAO) {           // 光照模式\n    Lux = (recvbuf[4]<<24|...|recvbuf[7]) / 100;\n    soil_moisture = Get_soil_moisture_percent();\n  } else {                           // 环境模式\n    WD  = (recvbuf[4]<<8|recvbuf[5]) / 100;  // 温度℃\n    QY  = (recvbuf[6]<<24|...|recvbuf[9])/100; // 气压Pa\n    SD  = (recvbuf[10]<<8|recvbuf[11]) / 100;  // 湿度%\n    HB  = recvbuf[12]<<8|recvbuf[13];          // 海拔m\n  }\n}\n// 土壤湿度: ADC1_IN8(PB0), 12位SAR, 线性映射\nuint8_t Get_soil_moisture_percent(void) {\n  uint16_t adc = ADC1_IN8_Read();\n  if (adc >= dry) return 0;          // 干土\n  if (adc <= wet) return 100;        // 湿土\n  return (dry - adc) * 100 / (dry - wet); // 0~100%\n}`,
    },
];

// ==================== 构建三线表 ====================
const col1Width = 1600;
const col2Width = 7760;  // 9360 - 1600
const tableWidth = col1Width + col2Width;

// 表头行
const headerRow = new TableRow({
    tableHeader: true,
    children: [
        codeCell("序号", col1Width, true),
        codeCell("代码模块", col1Width, true),
    ],
});

// 数据行
const dataRows = codeData.map((item, index) => {
    const isLast = index === codeData.length - 1;
    return new TableRow({
        children: [
            new TableCell({
                borders: isLast ? lastBorders : midBorders,
                width: { size: col1Width, type: WidthType.DXA },
                margins: cellMargins,
                verticalAlign: "center",
                children: [
                    new Paragraph({
                        alignment: AlignmentType.CENTER,
                        children: [new TextRun({ text: item.id, font: "Times New Roman", size: 20, bold: true })],
                    }),
                ],
            }),
            new TableCell({
                borders: isLast ? lastBorders : midBorders,
                width: { size: col2Width, type: WidthType.DXA },
                margins: cellMargins,
                verticalAlign: "top",
                children: [
                    // 模块名称（粗体）
                    new Paragraph({
                        spacing: { before: 40, after: 20 },
                        children: [new TextRun({ text: item.module, font: "黑体", size: 20, bold: true })],
                    }),
                    // 代码内容
                    ...item.code.split('\n').map(line =>
                        new Paragraph({
                            spacing: { before: 10, after: 10, line: 260 },
                            children: [
                                new TextRun({
                                    text: line || " ",
                                    font: "Consolas",
                                    size: 17,
                                }),
                            ],
                        })
                    ),
                ],
            }),
        ],
    });
});

// 构建表格
const table = new Table({
    width: { size: tableWidth, type: WidthType.DXA },
    columnWidths: [col1Width, col2Width],
    rows: [headerRow, ...dataRows],
});

// ==================== 构建文档 ====================
const doc = new Document({
    styles: {
        default: {
            document: {
                run: { font: "宋体", size: 24 }, // 小四 12pt
            },
        },
    },
    sections: [{
        properties: {
            page: {
                size: {
                    width: 11906,  // A4
                    height: 16838,
                },
                margin: {
                    top: 1134,    // 2cm
                    bottom: 1134,
                    left: 1701,   // 2.5cm (左右可调)
                    right: 1701,
                },
            },
        },
        children: [
            // 表序与表题
            new Paragraph({
                alignment: AlignmentType.CENTER,
                spacing: { before: 200, after: 200 },
                children: [
                    new TextRun({ text: "表X  系统核心代码（关键片段）", font: "黑体", size: 22, bold: true }),
                ],
            }),
            table,
            // 表注
            new Paragraph({
                spacing: { before: 100, after: 200 },
                children: [
                    new TextRun({
                        text: "注：以上代码为核心功能的关键片段，完整实现详见项目源文件。",
                        font: "宋体",
                        size: 18,
                        italics: true,
                    }),
                ],
            }),
        ],
    }],
});

// ==================== 输出 ====================
Packer.toBuffer(doc).then(buffer => {
    const outPath = "D:/Users/刘杰/Desktop/毕业论文/毕业设计代码/基础版本/核心代码_三线表.docx";
    fs.writeFileSync(outPath, buffer);
    console.log("三线表已生成: " + outPath);
});
