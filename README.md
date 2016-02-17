# LimeReport

##How to use it
1. Build limereport.pro. It will create a limereport shared library  
2. In your project connect the limereport library then in source code add:
 
```C
  #include "lrreportengine.h" to add report engine 
  #include "lrcallbackdatasourceintf.h" if you want use callback datasources

  report = new LimeReport::ReportEngine(this); to create reportengine
  report->dataManager()->addModel("string_list",stringListModel,true); to add datasource to report engine
  report->loadFromFile("File name"); to load report template file
  report->previewReport(); to generate report and preview
  report->printReport(); to print report 	 

```
For more samples see a demo