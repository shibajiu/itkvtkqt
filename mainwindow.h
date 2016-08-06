#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt includes
#include <QMainWindow>
#include <QVTKWidget.h>
#include <QVTKApplication.h>

// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
// needed to easily convert int to std::string
#include <sstream>
#include <string>
//itk include
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageToVTKImageFilter.h"

namespace Ui {
	class MainWindow;
	class StatusMessage;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

private slots:
	void on_openBt1_clicked();
};

class StatusMessage{
public:
	static std::string Format(int, int);
};
#endif // MAINWINDOW_H
