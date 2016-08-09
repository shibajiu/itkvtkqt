#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt includes
#include <QMainWindow>
#include <QFileDialog>
#include <QString>

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

class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
	static myVtkInteractorStyleImage* New();
	vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
	vtkImageViewer2* _ImageViewer;
	vtkTextMapper* _StatusMapper;
	int _Slice;
	int _MinSlice;
	int _MaxSlice;

public:
	void SetImageViewer(vtkImageViewer2* imageViewer);

	void SetStatusMapper(vtkTextMapper* statusMapper);


protected:
	void MoveSliceForward();

	void MoveSliceBackward();
	virtual void OnKeyDown();


	virtual void OnMouseWheelForward();
	virtual void OnMouseWheelBackward();
};


#endif // MAINWINDOW_H
