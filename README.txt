What is propertystore?
Propertystore enables applications to easily store object properties and other information in a database and to display that information to the user via an included Qt widget. Propertystore is a cross-platform C++ library which has been built and tested on Windows, GNU/Linux, and Mac OSX.

What does propertystore offer?
Propertystore offers a quick and straightforward way to define groups of related properties, add constraints on allowable values, and connect property changes to validation code. Propertystore offers tools that look through these groups of properties to determine what to show the user, and automatically builds a user interface to display property data and allow user interaction with the data. Propertystore uses the crunchstore library to handle saving sets of properties to a database and reading them back from a database. Application database interaction is usually along the lines of MyPropertyset.Save() and MyPropertyset.Load(). You don't have to deal with SQL queries or worry about how to translate your data to and from the database.

Who should use propertystore?
If you've ever wanted to store object or application information in a database and be able to display that information to the user for interaction, propertystore has something to offer. You don't have to deal directly with SQL queries, data normalization, or all the headaches of converting database entries into meaningful user interface elements. Propertystore allows you to deal with your data as coherent C++ objects rather than as "entries in a database."

Who should not use propertystore?
If the data your application deals with is currently accessed using complicated queries involving lots of JOIN statements, propertystore is probably not a good fit. Propertystore is better thought of as an object-relational mapper with extra features. As such, it's very good for simple data models where object persistence is the key, but not so good for more complicated data models in which you generally want to work with the interrelated pieces of a large group of objects.

Which databases are supported?
Propertystore uses the crunchstore library to interact with databases, providing support for SQLite and MongoDB right out of the box. Crunchstore can be readily extended to support other databases as well as other data storage techniques such as the Windows Registry, flat text files, and xml files.
