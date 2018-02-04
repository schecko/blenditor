#include "Camera_positions_list.h"

#include "ui_Camera_positions_list.h"
#include <QListView>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <CGAL/number_type_config.h>
#include <CGAL/Three/Viewer_interface.h>



#include <cassert>

Camera_positions_list::Camera_positions_list(QWidget* parent)
  : QDockWidget(parent), m_viewer(0), counter(0), m_model(new QStandardItemModel(this))
{
  Ui::Camera_positions_list ui;
  ui.setupUi(this);
  m_listView = ui.listView;
  m_listView->setModel(m_model);
  
  m_listView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  connect(m_listView, SIGNAL(activated(QModelIndex)),
          this, SLOT(activatedRow(QModelIndex)));
}

void Camera_positions_list::setViewer(CGAL::Three::Viewer_interface* viewer)
{
  m_viewer = viewer;
}

void Camera_positions_list::on_plusButton_pressed()
{
  if(!m_viewer) return;
  addItem(tr("Camera Position #%1").arg(++counter),
          m_viewer->dumpCameraCoordinates());
}

void Camera_positions_list::addItem(QString text, QString data)
{
  QStandardItem* item = new QStandardItem(text);
  item->setData(data, Qt::UserRole);
  m_model->insertRow(m_model->rowCount(), item);
}

void Camera_positions_list::on_upButton_pressed()
{
  int row = m_listView->selectionModel()->currentIndex().row();
  if(row!=0)
  {
  m_model->insertRow(row-1, m_model->takeRow(row));
  m_listView->selectionModel()->setCurrentIndex(m_model->index(row-1, 0),
                                                QItemSelectionModel::Clear);
  }
}

void Camera_positions_list::on_downButton_pressed()
{
  int row = m_listView->selectionModel()->currentIndex().row();
  if(row!= m_listView->model()->rowCount()-1)
  {
  m_model->insertRow(row+1, m_model->takeRow(row));
  m_listView->selectionModel()->setCurrentIndex(m_model->index(row+1, 0),
                                                QItemSelectionModel::Clear);
  }
}

void Camera_positions_list::on_minusButton_pressed()
{
  Q_FOREACH(QModelIndex index, 
            m_listView->selectionModel()->selectedIndexes()) {
    m_model->removeRows(index.row(), 1);
  }
}

void Camera_positions_list::on_clearButton_pressed()
{
  m_model->clear();
}

// void Camera_positions_list::editItem(QListWidgetItem* item)
// {
//   std::cerr << "is_editable: " << m_listView->flags(item)QListWidget

//   m_listView->editItem(item);
// }

void Camera_positions_list::activatedRow(QModelIndex index)
{
  QString s = m_model->data(index, Qt::UserRole).toString();
  if(s.isNull()) return;
  m_viewer->moveCameraToCoordinates(s);
}

void Camera_positions_list::on_saveButton_pressed()
{
  QString filename =
    QFileDialog::getSaveFileName(this, 
                                 tr("Save camera coordinates to file"),
                                 QString(),
                                 tr("(*.camera.txt)"));
  QFile file(filename);
  file.open(QIODevice::WriteOnly);
  QTextStream out(&file);
  for(int i = 0; i < m_model->rowCount(); ++i)
  {
    QStandardItem* item = m_model->item(i);
    out << item->data(Qt::DisplayRole).toString()
        << "\n"
        << item->data(Qt::UserRole).toString()
        << "\n";
  }
  file.close();
}

void Camera_positions_list::on_openButton_pressed()
{
  QString filename =
    QFileDialog::getOpenFileName(this, 
                                 tr("Read camera coordinates from file"),
                                 QString(),
                                 tr("(*.camera.txt)"));
  load(filename);
}

void Camera_positions_list::load(QString filename) {
  QFile file(filename);
  std::clog << "Loading camera positions " << qPrintable(filename) << std::endl;
  file.open(QIODevice::ReadOnly);
  QTextStream input(&file);
  while(!input.atEnd()) {
    QString text = input.readLine(1000);
    QString coord = input.readLine(1000);
    if(text.isNull() || coord.isNull()) return;
    qglviewer::Frame frame;
    if(m_viewer->readFrame(coord, frame))
    {
      addItem(text,
              m_viewer->dumpFrame(frame));
    }
  }
}

void Camera_positions_list::on_frontButton_pressed()
{
    qglviewer::Vec posFront = qglviewer::Vec(0,0,m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)));
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posFront = posFront + trans;
    qglviewer::Quaternion dirFront;
    dirFront.setAxisAngle(qglviewer::Vec(0,1,0),0);
    QString frontCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posFront[0])
            .arg(posFront[1])
            .arg(posFront[2])
            .arg(dirFront[0])
            .arg(dirFront[1])
            .arg(dirFront[2])
            .arg(dirFront[3]);

   m_viewer->moveCameraToCoordinates(frontCoord, 0.5f);
}

void Camera_positions_list::on_backButton_pressed()
{
    qglviewer::Vec posBack = qglviewer::Vec(0,0,-m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)));
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posBack+= trans;
    qglviewer::Quaternion dirBack;
    dirBack.setAxisAngle(qglviewer::Vec(0,1,0),CGAL_PI);
    QString backCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posBack[0])
            .arg(posBack[1])
            .arg(posBack[2])
            .arg(dirBack[0])
            .arg(dirBack[1])
            .arg(dirBack[2])
            .arg(dirBack[3]);
    m_viewer->moveCameraToCoordinates(backCoord, 0.5f);
}

void Camera_positions_list::on_topButton_pressed()
{
    qglviewer::Vec posTop = qglviewer::Vec(0,m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)), 0);
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posTop += trans;
    qglviewer::Quaternion dirTop;
    dirTop.setAxisAngle(qglviewer::Vec(1,0,0), -CGAL_PI/2);
    QString topCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posTop[0])
            .arg(posTop[1])
            .arg(posTop[2])
            .arg(dirTop[0])
            .arg(dirTop[1])
            .arg(dirTop[2])
            .arg(dirTop[3]);
     m_viewer->moveCameraToCoordinates(topCoord, 0.5f);
}

void Camera_positions_list::on_botButton_pressed()
{
    qglviewer::Vec posBot = qglviewer::Vec(0,-m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)), 0);;
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posBot += trans;
    qglviewer::Quaternion dirBot;
    dirBot.setAxisAngle(qglviewer::Vec(1,0,0),CGAL_PI/2);
    QString botCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posBot[0])
            .arg(posBot[1])
            .arg(posBot[2])
            .arg(dirBot[0])
            .arg(dirBot[1])
            .arg(dirBot[2])
            .arg(dirBot[3]);
     m_viewer->moveCameraToCoordinates(botCoord, 0.5f);
}

void Camera_positions_list::on_leftButton_pressed()
{
    qglviewer::Vec posLeft = qglviewer::Vec(-m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)), 0, 0);;
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posLeft += trans;
    qglviewer::Quaternion dirLeft;
    dirLeft.setAxisAngle(qglviewer::Vec(0,1,0),-CGAL_PI/2);
    QString leftCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posLeft[0])
            .arg(posLeft[1])
            .arg(posLeft[2])
            .arg(dirLeft[0])
            .arg(dirLeft[1])
            .arg(dirLeft[2])
            .arg(dirLeft[3]);
    m_viewer->moveCameraToCoordinates(leftCoord, 0.5f);
}

void Camera_positions_list::on_rightButton_pressed()
{
    qglviewer::Vec posRight = qglviewer::Vec(m_viewer->sceneRadius()/(sin (m_viewer->camera()->fieldOfView()/2)), 0,0);
#if QGLVIEWER_VERSION >= 0x020502
    qglviewer::Vec trans = m_viewer->camera()->pivotPoint();
#else
    qglviewer::Vec trans = m_viewer->camera()->revolveAroundPoint();
#endif
    posRight += trans;
    qglviewer::Quaternion dirRight;
    dirRight.setAxisAngle(qglviewer::Vec(0,1,0),CGAL_PI/2);
    QString rightCoord = QString("%1 %2 %3 %4 %5 %6 %7")
            .arg(posRight[0])
            .arg(posRight[1])
            .arg(posRight[2])
            .arg(dirRight[0])
            .arg(dirRight[1])
            .arg(dirRight[2])
            .arg(dirRight[3]);
    m_viewer->moveCameraToCoordinates(rightCoord, 0.5f);

}
