LimeReport v1.4.7    [![Build Status](https://api.travis-ci.org/fralx/LimeReport.svg?branch=master)](https://travis-ci.org/fralx/LimeReport)
-----------
Official LimeReport web site [http://limereport.ru](http://limereport.ru)
-----------

##Features
* Multi-platform support
* Pure Qt4/Qt5 compatible code
* Embedded report designer
* Embedded preview
* Various band types for a report of any complexity
* Page header and footer
* Data grouping (GroupHeader, GroupFooter, Subdetail, SubdetailHeader, SubdetailFooter)
* Aggregation functions (SUM, COUNT, AVG, MIN, MAX)
* Report elements: Text, Geometric (Line, Ellipsis, Rectangle), Picture
* Horizontal elements groups
* HTML to format input fields
* Scripts to format output data
* An Automatic band height adjustment
* A Smart band split moving data to the next page
* PDF output


##How to use it
1. Build limereport.pro. It will create a limereport shared library  
2. In your project connect the limereport library then in source code add:

```cpp
  #include "lrreportengine.h" to add report engine
  #include "lrcallbackdatasourceintf.h" if you want use callback datasources

  report = new LimeReport::ReportEngine(this); to create reportengine
  report->dataManager()->addModel("string_list",stringListModel,true); to add datasource to report engine
  report->loadFromFile("File name"); to load report template file
  report->previewReport(); to generate report and preview
  report->printReport(); to print report

```
For more samples see a demo

## Change log

###1.4.7
1. Multipage
2. Dialogs
3. Render events 
4. Initscript
5. Memory usage has been reduced 
6. Datasource manager has been refactored
7. Report items context menus have been added
8. Editable report
And many others minor fixes and improvements

###1.3.11
1. LimeReport project structure has been changed
2. Preview widget has been added
3. New demo has been added
4. Landscape page orientation has been fixed
5. Other minor bugs has been fixed

###1.3.10
1. Memory leak has been fixed
2. Grid & Settings have been added
3. Recent files menu has been added
4. Magnet feature has been added
5. Added ability to use variables in the connection settings

###1.3.9
New functions:
```cpp
  QString::saveToString(),
  loadFromString(const QString& report, const QString& name=""),
  QByteArray::saveToByteArray(),
  setCurrentReportsDir(const QString& dirName),
```
added to LimeReport::ReportEngine

1. printOnEach page and columns have been added to DataHeader band
2. startNewPage added to DataBand

Performance has been improved

**WARNING**
From this version the item "Text" by default does not use HTML.
To enable HTML support you need to use the property allowHTML

###1.3.1
Added:
1. Columns
   Some bands can be divided into columns
2. Items align
   Report items now may be aligned to the left,right or center of container
   also it can be stretched to the whole width of container
3. Group can start new page
4. Group can reset page number;
5. Table mode added to horizontal layout
   This mode allows you to distribute the internal layout's space among grouped items

Fixed:
1. Postgresql connection
2. The error that prevented normal run of more than one instance of LimeReport::ReportEngine

###1.2.1
1. Added buttons to open / hide sidebars;
2. Improved look-and-feel of report elements to clarify designing process;
3. Printing to PDF added.  
4. Fixed bug in SQL-editor when it used variables in SQL expression;
5. Fixed bug of variable's initialization if it exists more than once in SQL expression;
6. .. and other minor bugs fixed.
