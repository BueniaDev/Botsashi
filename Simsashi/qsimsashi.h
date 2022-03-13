#ifndef SIMSASHI_MAIN_H
#define SIMSASHI_MAIN_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <Botsashi/botsashi.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace botsashi;
using namespace std;

class QSimsashi : public QMainWindow, public BotsashiInterface
{
    public:
	QSimsashi(QWidget *parent = NULL);
	~QSimsashi();

	uint16_t readWord(bool upper, bool lower, uint32_t addr);
	void writeWord(bool upper, bool lower, uint32_t addr, uint16_t data);

	bool istrapOverride(int val);
	void trapException(int val);
	void runCore();
	void stopFunction();

	bool loadSRecord(string filename);

    private:
	Botsashi m68k;

	bool stopped = false;
	bool quit = false;

	QTimer *timer = NULL;

	void render();

	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

	vector<uint8_t> memory;

	uint32_t from_hex_str(string hex_str)
	{
	    return strtoul(hex_str.c_str(), NULL, 16);
	}

	QByteArray form_array(uint16_t num_bytes, int addr_reg = 1)
	{
	    QByteArray arr;
	    uint32_t arr_base = getAddrReg(addr_reg);

	    for (uint32_t index = 0; index < num_bytes; index++)
	    {
		uint32_t addr = (arr_base + index);
		arr.append(memory[addr]);
	    }

	    return arr;
	}

	QString form_string(int addr_reg = 1)
	{
	    QString str;
	    uint32_t str_base = getAddrReg(addr_reg);

	    for (uint32_t addr = str_base; (memory[addr] != 0); addr++)
	    {
		str.append(memory[addr]);
	    }

	    return str;
	}

	void set_string(QString str, int addr_reg = 2)
	{
	    uint32_t str_base = getAddrReg(addr_reg);

	    for (int index = 0; index < str.size(); index++)
	    {
		uint8_t character = str.at(index).toLatin1();
		uint32_t addr = (str_base + index);
		memory[addr] = character;
	    }

	    memory[(str_base + str.size())] = 0;
	}

	template<typename T = uint32_t>
	uint32_t getDataReg(int reg)
	{
	    switch (sizeof(T))
	    {
		case 1: return m68k.getDataReg<Botsashi::Byte>(reg); break;
		case 2: return m68k.getDataReg<Botsashi::Word>(reg); break;
		default: return m68k.getDataReg<Botsashi::Long>(reg); break;
	    }
	}

	template<typename T>
	void setDataReg(int reg, T val)
	{
	    switch (sizeof(T))
	    {
		case 1: m68k.setDataReg<Botsashi::Byte>(reg, val); break;
		case 2: m68k.setDataReg<Botsashi::Word>(reg, val); break;
		default: m68k.setDataReg<Botsashi::Long>(reg, val); break;
	    }
	}

	template<typename T = uint32_t>
	uint32_t getAddrReg(int reg)
	{
	    switch (sizeof(T))
	    {
		case 1: return m68k.getAddrReg<Botsashi::Byte>(reg); break;
		case 2: return m68k.getAddrReg<Botsashi::Word>(reg); break;
		default: return m68k.getAddrReg<Botsashi::Long>(reg); break;
	    }
	}

	vector<uint8_t> framebuffer;

	uint32_t fetch_version(uint8_t major, uint8_t minor, uint8_t patch)
	{
	    return ((major << 16) | (minor << 8) | patch);
	}

	QImage constructImage()
	{
	    return QImage(framebuffer.data(), 640, 480, QImage::Format_RGB888);
	}

	QPainter constructPainter(QImage &image)
	{
	    return QPainter(&image);
	}

	QString font_family;
	int font_size;

	void textOut(QString str);
	void textOutCRLF(QString str);
	void charOut(QPainter &painter, QChar character);
	void setTextSize();

	void backspace(QPainter &painter);
	void drawChar(QPainter &painter, int x, int y, QChar character);
	void drawCharInternal(QPainter &painter, int x, int y, QChar character, bool is_print);

	void setPenColor(uint32_t color);
	void setFillColor(uint32_t color);
	void setPixel(int xpos, int ypos);
	uint32_t getPixel(int xpos, int ypos);

	void drawLine(int x1, int y1, int x2, int y2);
	void drawRect(int left, int top, int right, int bottom);
	void drawEllipse(int left, int top, int right, int bottom);
	void drawUnfilledRect(int left, int top, int right, int bottom);
	void drawUnfilledEllipse(int left, int top, int right, int bottom);
	void lineTo(int xpos, int ypos);
	void moveTo(int xpos, int ypos);

	void setPenWidth(int width);

	void doCRLF();
	void setTextPos(int row, int col);

	QPainter img_painter;

	int current_row = 0;
	int current_col = 0;

	int window_width = 0;
	int window_height = 0;

	int font_width = 0;
	int font_height = 0;

	int text_xpos = 0;
	int text_ypos = 0;

	QPoint current_point;

	QString line_text;

	void setFont(QString family, int size);

	QPen current_pen;
	QBrush current_brush;

	bool key_pending = false;
	QString pending_text;

	array<QChar, (256 * 128)> characters;
};


#endif // SIMSASHI_MAIN_H