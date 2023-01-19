
# LimeReport v1.5.87 [![Build status](https://ci.appveyor.com/api/projects/status/wna5429pix7ilcmo/branch/master?svg=true)](https://ci.appveyor.com/project/fralx/limereport/branch/master) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/bc31412ea4814f30825b5ed3723e9a70)](https://app.codacy.com/app/fralx/LimeReport?utm_source=github.com&utm_medium=referral&utm_content=fralx/LimeReport&utm_campaign=Badge_Grade_Dashboard) ![Cmake Build Status](https://github.com/fralx/limereport/actions/workflows/cmake.yml/badge.svg)

## Official LimeReport web site [http://limereport.ru](http://limereport.ru)

## Features

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

### How to use it

#### QMake

- Build limereport.pro. It will create a limereport shared library  
- In your project connect the limereport library

#### CMake

To use in your application without installation

There are 2 possible ways:

- Use cmake subdirectory in your CMakeLists.txt:

```cmake
add_subdirectory(LimeReport)
target_link_libraries(myapp PRIVATE limereport-qt${QT_VERSION_MAJOR})
```
- Use cmake FetchContent in your CMakeLists.txt:

```cmake
include(FetchContent)
FetchContent_Declare(
  LimeReport
  GIT_REPOSITORY https://github.com/fralx/LimeReport.git
  GIT_TAG        sha-of-the-commit
)
FetchContent_MakeAvailable(LimeReport)
target_link_libraries(myapp PRIVATE limereport-qt${QT_VERSION_MAJOR})
```

- Then in source code add:

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

### Change log

#### 1.5.0

1. Added the ability to use QJSEngine instead of deprecated QtScript.
2. Report designer has been improved.
3. Inches support has been added.
4. Embedded dialog designer has been added.
5. The script editor has been improved.
6. Added the ability to build only report generator without embedded visual report designer.
7. Report translation ability has been added.
8. Added report generation time events with the ability to process them in the report script.
9. Added the ability to build a report table of contents.
10. The vertical layout has been added.
11. Added the ability to transfer an image to the report via a variable.
12. Endless height has been added.
13. Added the ability to print a report page on multiple pages of paper.
14. Added the ability to print on multiple printers.
15. ChartItem has been added.
16. Added the ability to use aggregate functions in headers.
17. Subtotals.
18. Dark and Light themes have been added to report designer.
19. Generation result editing mode has been improved.
20. And many other minor fixes and improvements.

#### 1.4.7

1. Multipage.
2. Dialogs.
3. Render events.
4. Initscript.
5. Memory usage has been reduced.
6. Data source manager has been refactored.
7. Report items context menus have been added.
8. Editable report.
9. And many other minor fixes and improvements.

#### 1.3.11

1. The LimeReport project structure has been changed.
2. Preview widget has been added.
3. A new demo has been added.
4. Landscape page orientation has been fixed.
5. Other minor bugs have been fixed.

#### 1.3.10

1. A memory leak has been fixed.
2. Grid & Settings have been added.
3. Recent files menu has been added.
4. Magnet feature has been added.
5. Added ability to use variables in the connection settings.

#### 1.3.9

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
From this version, the item "Text" by default does not use HTML.
To enable HTML support you need to use the property allowHTML

#### 1.3.1

Added:

1. Columns
   Some bands can be divided into columns
2. Items align
   Report items now may be aligned to the left, right or center of the container
   also it can be stretched to the whole width of the container
3. Group can start a new page
4. Group can reset page number;
5. Table mode added to the horizontal layout
   This mode allows you to distribute the internal layout's space among grouped items

Fixed:

1. Postgresql connection
2. The error that prevented the normal run of more than one instance of LimeReport::ReportEngine

#### 1.2.1

1. Added buttons to open / hide sidebars;
2. Improved look-and-feel of report elements to clarify the designing process;
3. Printing to PDF added.  
4. Fixed bug in SQL-editor when it used variables in SQL expression;
5. Fixed bug of variable's initialization if it exists more than once in SQL expression;
6. .. and other minor bugs fixed.
