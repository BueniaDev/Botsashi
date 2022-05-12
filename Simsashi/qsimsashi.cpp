#include "qsimsashi.h"

QSimsashi::QSimsashi(QWidget *parent) : window_width(640), window_height(480)
{
    current_pen.setCapStyle(Qt::RoundCap);
    current_pen.setJoinStyle(Qt::RoundJoin);
    current_brush.setStyle(Qt::SolidPattern);
    memory.resize(0x1000000, 0x00);
    m68k.setinterface(*this);

    setFont("Fixedsys", 9);

    framebuffer.resize((window_width * window_height * 3), 0x00);

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this]{
	render();
    });

    resize(window_width, window_height);
}

QSimsashi::~QSimsashi()
{

}

uint16_t QSimsashi::readWord(bool upper, bool lower, uint32_t addr)
{
    uint16_t data = 0;

    if (upper)
    {
	data |= (memory.at(addr) << 8);
    }

    if (lower)
    {
	data |= memory.at(addr + 1);
    }

    return data;
}

void QSimsashi::writeWord(bool upper, bool lower, uint32_t addr, uint16_t data)
{
    if (upper)
    {
	memory.at(addr) = (data >> 8);
    }

    if (lower)
    {
	memory.at(addr + 1) = (data & 0xFF);
    }
}

bool QSimsashi::istrapOverride(int val)
{
    return (val == 15);
}

void QSimsashi::trapException(int val)
{
    if (val != 15)
    {
	return;
    }

    auto function_number = getDataReg<uint8_t>(0);

    switch (function_number)
    {
	case 2:
	{
	    QString text;
	    if (key_pending)
	    {
		key_pending = false;
		text = pending_text;
		pending_text = "";
	    }
	    else
	    {
		while (true)
		{
		    text = QInputDialog::getText(this, "Simsashi", "Enter a line of text here:");

		    if ((text.size() >= 0) && (text.size() < 80))
		    {
			break;
		    }
		}
	    }

	    textOutCRLF(text);
	    set_string(text, 1);
	    setDataReg<uint32_t>(1, text.size());
	}
	break;
	case 4:
	{
	    if (key_pending)
	    {
		cout << "Key pending" << endl;
		exit(0);
	    }
	    else
	    {
		QString text = QInputDialog::getText(this, "Simsashi", "Enter a number here:");
		int number = text.toInt();
		textOutCRLF(QString::number(number));
		setDataReg<uint32_t>(1, number);
	    }
	}
	break;
	case 5:
	{
	    QChar text_char;
	    if (key_pending)
	    {
		cout << "Key pending" << endl;
		exit(0);
	    }
	    else
	    {
		QString text;
		while (true)
		{
		    text = QInputDialog::getText(this, "Simsashi", "Enter ONE ASCII character here:");

		    if (text.size() == 1)
		    {
			break;
		    }
		}

		text_char = text.at(0);
	    }

	    textOutCRLF(text_char);
	    setDataReg<uint8_t>(1, text_char.toLatin1());
	}
	break;
	case 7:
	{
	    int key_value = (key_pending) ? 1 : 0;
	    setDataReg<uint8_t>(1, key_value);
	}
	break;
	case 9: stopFunction(); break;
	case 11:
	{
	    uint16_t value = getDataReg<uint16_t>(1);

	    switch (value)
	    {
		case 0xFF00:
		{
		    cout << "Clearing screen..." << endl;
		}
		break;
		case 0x00FF:
		{
		    cout << "Unimplemented: Get cursor position" << endl;
		    exit(0);
		}
		break;
		default:
		{
		    uint8_t xpos = (value >> 8);
		    uint8_t ypos = (value & 0xFF);

		    if (((xpos >= 0) && (xpos <= 255)) && ((ypos >= 0) || (ypos <= 128)))
		    {
			cout << "Setting cursor position to (" << dec << int(xpos) << "," << dec << int(ypos) << ")" << endl;
		    }
		}
		break;
	    }
	}
	break;
	case 13:
	{
	    QString text_str = form_string();
	    textOutCRLF(text_str);
	}
	break;
	case 14:
	{
	    QString text_str = form_string();
	    textOut(text_str);
	}
	break;
	case 23:
	{
	    uint32_t centiseconds = getDataReg<uint32_t>(1);
	    // cout << "Delaying " << dec << int(centiseconds) << " centiseconds" << endl;
	}
	break;
	case 32:
	{
	    auto subfunc_number = getDataReg<uint8_t>(1);

	    switch (subfunc_number)
	    {
		case 4:
		{
		    setDataReg<uint32_t>(1, fetch_version(5, 15, 0));
		}
		break;
		default:
		{
		    cout << "Unrecognized subfunction number of " << dec << int(subfunc_number) << endl;
		    exit(1);
		}
		break;
	    }
	}
	break;
	case 33:
	{
	    uint32_t value = getDataReg(1);

	    switch (value)
	    {
		case 0:
		{
		    cout << "Fetching current window resolution" << endl;
		    uint16_t width = window_width;
		    uint16_t height = window_height;

		    uint32_t res_val = ((width << 16) | height);
		    setDataReg(1, res_val);
		}
		break;
		case 1:
		{
		    cout << "Unimplemented: Windowed mode" << endl;
		}
		break;
		case 2:
		{
		    cout << "Unimplemented: Full-screen mode" << endl;
		}
		break;
		default:
		{
		    cout << "Unimplemented: Set window resolution" << endl;
		}
		break;
	    }
	}
	break;
	// TODO: File API
	case 50:
	{
	    cout << "Closing all files..." << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 51:
	{
	    QString filename = form_string();
	    cout << "Opening new file of " << filename.toStdString() << endl;
	    setDataReg<uint16_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	case 52:
	{
	    QString filename = form_string();
	    cout << "Opening new file of " << filename.toStdString() << endl;
	    setDataReg<uint16_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	case 53:
	{
	    cout << "Reading file..." << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 54:
	{
	    cout << "Writing file..." << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 55:
	{
	    cout << "Seeking file..." << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 56:
	{
	    cout << "Closing file..." << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 57:
	{
	    QString filename = form_string();
	    cout << "Closing file of " << filename.toStdString() << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 58:
	{
	    cout << "Displaying file dialog" << endl;
	    setDataReg<uint32_t>(1, 0);
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 59:
	{
	    QString filename = form_string();
	    cout << "Performing file operations on filename of " << filename.toStdString() << endl;
	    setDataReg<uint16_t>(0, 0);
	}
	break;
	case 60:
	{
	    uint16_t reg_value = getDataReg<uint16_t>(1);

	    mouse_irq_level = (reg_value >> 8);
	    mouse_irq_mask = (reg_value & 0xFF);
	}
	break;
	// TODO: Sound API
	case 70:
	{
	    QString wav_filename = form_string();
	    cout << "Playing WAV of " << wav_filename.toStdString() << endl;
	    setDataReg<uint16_t>(0, 1);
	}
	break;
	case 71:
	{
	    QString wav_filename = form_string();
	    cout << "Loading WAV of " << wav_filename.toStdString() << endl;
	    setDataReg<uint8_t>(1, 0);
	}
	break;
	case 72:
	{
	    auto sound_id = getDataReg<uint8_t>(1);
	    cout << "Playing sound " << dec << int(sound_id) << endl;
	    // D0.W is non-zero if the sound was played
	    setDataReg<uint16_t>(0, 1);
	}
	break;
	case 80:
	{
	    uint32_t color = getDataReg(1);
	    setPenColor(color);
	}
	break;
	case 81:
	{
	    uint32_t color = getDataReg(1);
	    setFillColor(color);
	}
	break;
	case 82:
	{
	    int xpos = getDataReg<uint16_t>(1);
	    int ypos = getDataReg<uint16_t>(2);

	    setPixel(xpos, ypos);
	}
	break;
	case 83:
	{
	    int xpos = getDataReg<uint16_t>(1);
	    int ypos = getDataReg<uint16_t>(2);
	    setDataReg(0, getPixel(xpos, ypos));
	}
	break;
	case 84:
	{
	    int x1 = getDataReg<uint16_t>(1);
	    int y1 = getDataReg<uint16_t>(2);
	    int x2 = getDataReg<uint16_t>(3);
	    int y2 = getDataReg<uint16_t>(4);

	    drawLine(x1, y1, x2, y2);
	}
	break;
	case 85:
	{
	    int xpos = getDataReg<uint16_t>(1);
	    int ypos = getDataReg<uint16_t>(2);
	    lineTo(xpos, ypos);
	}
	break;
	case 86:
	{
	    int xpos = getDataReg<uint16_t>(1);
	    int ypos = getDataReg<uint16_t>(2);

	    moveTo(xpos, ypos);
	}
	break;
	case 87:
	{
	    int left = getDataReg<uint16_t>(1);
	    int top = getDataReg<uint16_t>(2);
	    int right = getDataReg<uint16_t>(3);
	    int bottom = getDataReg<uint16_t>(4);
	    drawRect(left, top, right, bottom);
	}
	break;
	case 88:
	{
	    int left = getDataReg<uint16_t>(1);
	    int top = getDataReg<uint16_t>(2);
	    int right = getDataReg<uint16_t>(3);
	    int bottom = getDataReg<uint16_t>(4);
	    drawEllipse(left, top, right, bottom);
	}
	break;
	case 89:
	{
	    // TODO: Implement flood filling
	    int xpos = getDataReg<uint16_t>(1);
	    int ypos = getDataReg<uint16_t>(2);
	    cout << "Unimplemented: Flood filling area at (" << dec << int(xpos) << ", " << dec << int(ypos) << ")" << endl;
	}
	break;
	case 90:
	{
	    int left = getDataReg<uint16_t>(1);
	    int top = getDataReg<uint16_t>(2);
	    int right = getDataReg<uint16_t>(3);
	    int bottom = getDataReg<uint16_t>(4);
	    drawUnfilledRect(left, top, right, bottom);
	}
	break;
	case 91:
	{
	    int left = getDataReg<uint16_t>(1);
	    int top = getDataReg<uint16_t>(2);
	    int right = getDataReg<uint16_t>(3);
	    int bottom = getDataReg<uint16_t>(4);
	    drawUnfilledEllipse(left, top, right, bottom);
	}
	break;
	case 92:
	{
	    uint8_t mode_num = getDataReg<uint8_t>(1);
	    cout << "Unimplemented: Setting drawing mode to " << dec << int(mode_num) << endl;
	}
	break;
	case 93:
	{
	    int pen_width = getDataReg<uint8_t>(1);
	    setPenWidth(pen_width);
	}
	break;
	case 95:
	{
	    QString text_str = form_string();

	    uint32_t xpos = getDataReg<uint16_t>(1);
	    uint32_t ypos = getDataReg<uint16_t>(2);

	    cout << "Unimplemented: Drawing text" << endl;
	    cout << "Position: (" << dec << xpos << "," << dec << ypos << ")" << endl;
	    cout << "Text string: " << text_str.toStdString() << endl;
	    cout << endl;
	}
	break;
	case 100:
	{
	    cout << "Configuring as client..." << endl;
	    uint32_t config_reg = getDataReg<uint32_t>(1);

	    int mode = (config_reg & 0xFF);
	    int port_number = (config_reg >> 16);

	    QString ip_addr = form_string(2);

	    switch (mode)
	    {
		case 0:
		{
		    cout << "Setting up UDP connection to address of " << ip_addr.toStdString() << " and port of " << dec << port_number << endl;
		}
		break;
		case 1:
		{
		    cout << "Setting up TCP connection to address of " << ip_addr.toStdString() << " and port of " << dec << port_number << endl;
		}
		break;
		default:
		{
		    cout << "Reserved value of " << dec << mode << " detected" << endl;
		}
		break;
	    }

	    setDataReg<uint32_t>(0, 0);
	}
	break;
	case 101:
	{
	    cout << "Configuring as server..." << endl;
	    uint32_t config_reg = getDataReg<uint32_t>(1);

	    int mode = (config_reg & 0xFF);
	    int port_number = (config_reg >> 16);

	    switch (mode)
	    {
		case 0:
		{
		    cout << "Setting up UDP connection to port of " << dec << port_number << endl;
		}
		break;
		case 1:
		{
		    cout << "Setting up TCP connection to port of " << dec << port_number << endl;
		}
		break;
		default:
		{
		    cout << "Reserved value of " << dec << mode << " detected" << endl;
		}
		break;
	    }

	    setDataReg<uint32_t>(0, 0);
	}
	break;
	case 102:
	{
	    cout << "Sending data" << endl;
	    setDataReg<uint32_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	case 103:
	{
	    cout << "Receiving data" << endl;
	    QString temp_ip = "127.0.0.1";

	    set_string(temp_ip);

	    setDataReg<uint32_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	case 105:
	{
	    cout << "Fetching local IP address..." << endl;
	    QString temp_ip = "127.0.0.1";

	    set_string(temp_ip);

	    setDataReg<uint32_t>(0, 0);
	}
	break;
	case 106:
	{
	    cout << "Sending data on specified port" << endl;
	    setDataReg<uint32_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	case 107:
	{
	    cout << "Receiving data and port number" << endl;
	    QString temp_ip = "127.0.0.1";

	    set_string(temp_ip);

	    setDataReg<uint32_t>(0, 0);
	    setDataReg<uint32_t>(1, 0);
	}
	break;
	default:
	{
	    cout << "Unrecognized I/O function number of " << dec << function_number << endl;
	    exit(1);
	}
	break;
    }

    repaint();
}

void QSimsashi::stopFunction()
{
    cout << "Stopping Simsashi..." << endl;
    stopped = true;
}

void QSimsashi::setPenColor(uint32_t color)
{
    int red = (color & 0xFF);
    int green = ((color >> 8) & 0xFF);
    int blue = ((color >> 16) & 0xFF);

    current_pen.setColor(QColor(red, green, blue));
}

void QSimsashi::setFillColor(uint32_t color)
{
    int red = (color & 0xFF);
    int green = ((color >> 8) & 0xFF);
    int blue = ((color >> 16) & 0xFF);

    current_brush.setColor(QColor(red, green, blue));
}

void QSimsashi::setPixel(int xpos, int ypos)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    painter.setPen(current_pen);
    painter.setBrush(current_brush);

    painter.drawPoint(xpos, ypos);
}

uint32_t QSimsashi::getPixel(int xpos, int ypos)
{
    QImage image = constructImage();

    QColor color = image.pixelColor(xpos, ypos);

    return (color.red() | (color.green() << 8) | (color.blue() << 16));
}

void QSimsashi::moveTo(int xpos, int ypos)
{
    current_point = QPoint(xpos, ypos);
}

void QSimsashi::drawLine(int x1, int y1, int x2, int y2)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    painter.setPen(current_pen);
    painter.drawLine(x1, y1, x2, y2);
    current_point = QPoint(x2, y2);
}

void QSimsashi::lineTo(int xpos, int ypos)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    painter.setPen(current_pen);
    painter.drawLine(current_point.x(), current_point.y(), xpos, ypos);
    current_point = QPoint(xpos, ypos);
}

void QSimsashi::drawRect(int left, int top, int right, int bottom)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    QRect rect;
    rect.setCoords(left, top, right, bottom);
    painter.setPen(current_pen);
    painter.setBrush(current_brush);
    painter.drawRect(rect);
}

void QSimsashi::drawEllipse(int left, int top, int right, int bottom)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    QRect rect;
    rect.setCoords(left, top, right, bottom);
    painter.setPen(current_pen);
    painter.setBrush(current_brush);
    painter.drawEllipse(rect);
}

void QSimsashi::drawUnfilledRect(int left, int top, int right, int bottom)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    QRect rect;
    rect.setCoords(left, top, right, bottom);
    painter.setPen(current_pen);
    painter.drawRect(rect);
}

void QSimsashi::drawUnfilledEllipse(int left, int top, int right, int bottom)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);

    QRect rect;
    rect.setCoords(left, top, right, bottom);
    painter.setPen(current_pen);
    painter.drawEllipse(rect);
}

void QSimsashi::setPenWidth(int width)
{
    current_pen.setWidth(width);
}

void QSimsashi::textOutCRLF(QString str)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);
    QFont img_font(font_family, font_size);
    painter.setFont(img_font);
    painter.setPen(Qt::white);

    for (int index = 0; index < str.size(); index++)
    {
	charOut(painter, str.at(index));
    }

    doCRLF();

    painter.setPen(current_pen);
}

void QSimsashi::textOut(QString str)
{
    QImage image = constructImage();
    QPainter painter = constructPainter(image);
    QFont img_font(font_family, font_size);
    painter.setFont(img_font);
    painter.setPen(Qt::white);

    for (int index = 0; index < str.size(); index++)
    {
	charOut(painter, str.at(index));
    }

    painter.setPen(current_pen);
}

void QSimsashi::charOut(QPainter &painter, QChar character)
{
    char latin_str = character.toLatin1();

    switch (latin_str)
    {
	case 0x07:
	{
	    cout << "Bell" << endl;
	}
	break;
	case 0x08:
	{
	    backspace(painter);
	}
	break;
	case 0x09:
	{
	    current_col += 5;
	}
	break;
	case 0x0A:
	{
	    current_row += 1;
	}
	break;
	case 0x0B:
	{
	    current_row += 4;
	}
	break;
	case 0x0C:
	{
	    cout << "Form feed" << endl;
	}
	break;
	case 0x0D:
	{
	    current_col = 0;
	}
	break;
	default:
	{
	    if ((latin_str < 0x00) || (latin_str >= 0x20))
	    {
		drawChar(painter, current_col, current_row, character);
		current_col += 1;
	    }
	}
	break;
    }

    setTextPos(current_row, current_col);
}

void QSimsashi::backspace(QPainter &painter)
{
    setTextPos(current_row, (current_col - 1));
    drawCharInternal(painter, current_col, current_row, 0, false);
}

void QSimsashi::drawChar(QPainter &painter, int x, int y, QChar character)
{
    drawCharInternal(painter, x, y, character, true);
}

void QSimsashi::drawCharInternal(QPainter &painter, int x, int y, QChar character, bool is_print)
{
    auto &prev_char = characters.at(x + (font_width * y));
    painter.setPen(Qt::black);
    painter.drawText(text_xpos, text_ypos, font_width, font_height, 0, prev_char);

    if (is_print)
    {
	prev_char = character;
	painter.setPen(Qt::white);
	painter.drawText(text_xpos, text_ypos, font_width, font_height, 0, character);
    }
}

void QSimsashi::setFont(QString family, int size)
{
    font_family = family;
    font_size = size;
    setTextSize();
}

void QSimsashi::setTextSize()
{
    QFont font(font_family, font_size);
    QFontMetrics fm(font);

    font_width = fm.horizontalAdvance("W");
    font_height = fm.height();
}

void QSimsashi::setTextPos(int row, int col)
{
    if (row < 0)
    {
	row = 0;
    }

    if (col < 0)
    {
	col = 0;
    }

    current_row = row;
    current_col = col;

    text_ypos = (font_height * row);
    text_xpos = (font_width * col);
}

void QSimsashi::doCRLF()
{
    current_col = 0;
    current_row += 1;

    setTextPos(current_row, current_col);
}

bool QSimsashi::loadSRecord(string filename)
{
    ifstream file(filename, ios::in);

    if (!file.is_open())
    {
	cout << "Could not load s-record file of " << filename << endl;
	return false;
    }

    string line;

    while (getline(file, line))
    {
	if (line[0] != 'S')
	{
	    cout << "Invalid S-Record" << endl;
	    return false;
	}

	if (!isdigit(line[1]))
	{
	    cout << "Unsupported record type of " << line[1] << endl;
	    return false;
	}

	uint8_t byte_count = from_hex_str(line.substr(2, 2));

	switch (line[1])
	{
	    case '0': break;
	    case '1':
	    {
		uint32_t code_addr = from_hex_str(line.substr(4, 4));
		uint32_t num_bytes = (byte_count - 3);

		for (size_t index = 0; index < num_bytes; index++)
		{
		    uint32_t byte_offs = (8 + (index << 1));
		    uint8_t data_byte = from_hex_str(line.substr(byte_offs, 2));
		    memory.at(code_addr + index) = data_byte;
		}
	    }
	    break;
	    case '2':
	    {
		uint32_t code_addr = from_hex_str(line.substr(4, 6));
		uint32_t num_bytes = (byte_count - 4);

		for (size_t index = 0; index < num_bytes; index++)
		{
		    uint32_t byte_offs = (10 + (index << 1));
		    uint8_t data_byte = from_hex_str(line.substr(byte_offs, 2));
		    memory.at(code_addr + index) = data_byte;
		}
	    }
	    break;
	    case '3':
	    {
		uint32_t code_addr = from_hex_str(line.substr(4, 8));
		uint32_t num_bytes = (byte_count - 5);

		for (size_t index = 0; index < num_bytes; index++)
		{
		    uint32_t byte_offs = (12 + (index << 1));
		    uint8_t data_byte = from_hex_str(line.substr(byte_offs, 2));
		    memory.at(code_addr + index) = data_byte;
		}
	    }
	    break;
	    case '5': break;
	    case '7':
	    {
		uint32_t exec_addr = from_hex_str(line.substr(4, 8));
		m68k.init(exec_addr);
	    }
	    break;
	    case '8':
	    {
		uint32_t exec_addr = from_hex_str(line.substr(4, 6));
		m68k.init(exec_addr);
	    }
	    break;
	    case '9':
	    {
		uint32_t exec_addr = from_hex_str(line.substr(4, 4));
		m68k.init(exec_addr);
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized type of S" << line[1] << endl;
		return false;
	    }
	    break;
	}
    }

    file.close();

    return true;
}

void QSimsashi::runCore()
{
    timer->start(1);
}

void QSimsashi::render()
{
    if (!stopped)
    {
	m68k.debugoutput();
	m68k.executenextinstr();
    }
}

void QSimsashi::closeEvent(QCloseEvent *event)
{
    m68k.shutdown();
    QMainWindow::closeEvent(event);
}

void QSimsashi::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
	cout << "Input text is " << pending_text.toStdString() << endl;
	doCRLF();
	key_pending = true;
    }
    else if (event->key() == Qt::Key_Backspace)
    {
	pending_text.chop(1);
    }
    else
    {
	pending_text.append(event->text().at(0));
	textOut(event->text().at(0));
    }
}

void QSimsashi::mousePressEvent(QMouseEvent *event)
{
    cout << "Mouse pressed" << endl;

    if (testbit(mouse_irq_mask, 0))
    {
	cout << "Firing mouse IRQ..." << endl;
	m68k.fire_irq(mouse_irq_level, (mouse_irq_level != 0));
    }
}

void QSimsashi::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    auto size = this->size();

    QRectF target(0, 0, size.rwidth(), size.rheight());
    QImage image = constructImage();
    QRectF source(0, 0, 640, 480);
    painter.drawImage(target, image, source);
}