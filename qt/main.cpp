#include <QApplication>
#include <QWidget>
#include <QPalette>
#include <QLabel>
#include <QPoint>
#include <QtGui>
#include <fstream>
#include <string>

#define WIDTH		420
#define HEIGHT	80

class Thread : public QThread {
    public:
        void run();
				QPalette p;
				QWidget *w;
};

static QLabel *label;

int main(int argc, char **argv){
  QApplication app(argc, argv);
	QWidget w;
	QPalette p = w.palette();
  p.setColor(QPalette::Window, Qt::blue);
  w.setPalette(p);
  w.setAutoFillBackground(true);
  w.showFullScreen();
  



  label = new QLabel;	
  label -> resize(WIDTH, HEIGHT);
  label -> setAlignment(Qt::AlignCenter);
	label -> QWidget::move((480 - WIDTH)/2, (272 - HEIGHT)/2);
  label->show();

	// DO IT
	Thread *thread = new Thread();
	thread->w = &w;
	thread->p = p;
  thread->start();


  return app.exec();
}

void Thread::run(){
	std::string line;
	std::ifstream myfile;
	while(true){
		myfile.open("/dev/mygpio", std::ifstream::in);
		if (myfile.is_open()){
		  getline (myfile,line);
		  myfile.close();
			label->setText(QString::fromStdString(line));
			if(line.compare("C\0") == 0){
				p.setColor(QPalette::Window, Qt::blue);
			} else if (line.compare("D\0") == 0){
				p.setColor(QPalette::Window, Qt::yellow);
			} else if (line.compare("E\0") == 0){
				p.setColor(QPalette::Window, Qt::green);
			} else if (line.compare("F\0") == 0){
				p.setColor(QPalette::Window, Qt::red);
			} else {
				p.setColor(QPalette::Window, Qt::black);
			}
			w->setPalette(p);
		} else {
			label->setText("ERROR\n");
		}
	}
}

