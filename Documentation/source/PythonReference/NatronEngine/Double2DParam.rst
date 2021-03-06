.. module:: NatronEngine
.. _Double2DParam:

Double2DParam
*************

**Inherits** :doc:`DoubleParam`

**Inherited by:** :ref:`Double3DParam`

Synopsis
--------

See :doc:`DoubleParam` for more informations on this class.

Functions
^^^^^^^^^

*    def :meth:`get<NatronEngine.Double2DParam.get>` ()
*    def :meth:`get<NatronEngine.Double2DParam.get>` (frame)
*    def :meth:`set<NatronEngine.Double2DParam.set>` (x, y)
*    def :meth:`set<NatronEngine.Double2DParam.set>` (x, y, frame)


Member functions description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. method:: NatronEngine.Double2DParam.get()
	
	:rtype: :class:`Double2DTuple`
	
Returns a :doc:`Double2DTuple` with the [x,y] values for this parameter at the current
timeline's time.



.. method:: NatronEngine.Double2DParam.get(frame)
	
	:rtype: :class:`Double2DTuple`
	
Returns a :doc:`Double2DTuple` with the [x,y] values for this parameter at the given *frame*.



.. method:: NatronEngine.Double2DParam.set(x, y, frame)


    :param x: :class:`float<PySide.QtCore.double>`
    :param y: :class:`float<PySide.QtCore.double>`
    :param frame: :class:`int<PySide.QtCore.int>`


Same as :func:`set(x,frame)<NatronEngine.DoubleParam.set>` but for 2-dimensional doubles.



.. method:: NatronEngine.Double2DParam.set(x, y)


    :param x: :class:`float<PySide.QtCore.double>`
    :param y: :class:`float<PySide.QtCore.double>`

Same as :func:`set(x)<NatronEngine.DoubleParam.set>` but for 2-dimensional doubles.





