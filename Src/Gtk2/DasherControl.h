#ifndef __dashercontrol_h__
#define __dashercontrol_h__

#include "PangoCache.h"
#include "Canvas.h"
#include "mouse_input.h"

#include "GtkDasherControl.h"

#include "../DasherCore/DasherSettingsInterface.h"
#include "../DasherCore/DashEdit.h"
#include "../DasherCore/DasherInterface.h"

///
/// \brief C++ core of the Dasher GTK UI component.
///
/// Class representing the Dasher UI component (ie the canvas and speed slider)
/// \todo It would really be more sensible for CDasherControl to inheret from CDasherInterface rather than contain a pointer to it
///

class CDasherControl : public CDasherSettingsInterface, public CDashEditbox {

 public:

  ///
  /// \param pVBox GTK VBox to populate with the DasherControl
  /// component widgets. This needs to be created externally by the
  /// GObject wrapper and passed to the C++ class rather than being
  /// created internally.  
  /// \param pDasherControl Pointer to the GObject wrapper. This is
  /// needed so that we can emit signals from the GObject.
  ///

  CDasherControl( GtkVBox *pVbox, GtkDasherControl *pDasherControl );
  ~CDasherControl();

  // Event handlers
  // FIXME - we should probably pass all parameters to these from the "C" callbacks

  ///
  /// Called when the canvas gets realized (ie when internal resources have been allocated), so we can finalise setup.
  ///

  void RealizeCanvas();

  ///
  /// Called periodically by a timer. Used to prompt the interface to perform a redraw
  /// \todo There's rather a lot which happens in this
  /// function. Ideally it should just be a simple call to the core
  /// which then figures out whether we're paused or not etc.
  ///

  int TimerEvent();

  ///
  /// Mouse button pressed on the canvas
  ///

  gboolean ButtonPressEvent(GdkEventButton *event);

  ///
  /// Called when the canvas has been resized, prompts the (re)creation of the CCanvas object.
  ///

  void CanvasConfigureEvent();

  ///
  /// Speed slider has been moved.
  ///

  void SliderEvent();

  ///
  /// Key press event on the canvas or the speed slider. This is
  /// currently a single handler for the two objects, but it may be
  /// sensible to at least partially split it in the future.
  /// \todo There's quite a bit of functionality which used to be here in 3.2 but which has now been removed. Some of this is obsolete button Dasher code, but some of it performs useful functions such as recalibration, so this needs to be re-implemented.
  ///

  gint KeyPressEvent( GdkEventKey *event );

  ///
  /// Scan for alphabet XML files in the system and user directories
  /// and add them to the interface.  
  /// \todo It seems odd that we have to do this externally to the
  /// interface. We really should make the interface do it at
  /// construction time.
  ///

  void scan_alphabet_files();

  ///
  /// As scan_alpahbet_files, but for colour schemes.
  ///

  void scan_colour_files();

  /// 
  /// \todo Pointless one-line function, bring in to scan_alphabet_files.
  ///

  int alphabet_filter(const gchar* filename, GPatternSpec *alphabet);

  ///
  /// \todo Pointless one-line function, bring in to scan_colour_files.
  ///

  int colour_filter(const gchar* filename, GPatternSpec *colourglob);
  
  ///
  /// Return an array of allowed values for a string parameter.
  /// \param iParameter The parameter to query.
  /// \return A GArray of gchar* pointers to strings containing permitted values
  ///

  GArray *GetAllowedValues( int iParameter );

  /// 
  /// Load a file and use to train the language model
  /// \param filename The file to load.
  ///

  void Train( const gchar *filename ) {
    m_pInterface->TrainFile( filename );
  };

  ///
  /// Returns a pointer the CDasherInterface object.
  ///

  CDasherInterface* GetInterface() {
    return m_pInterface;
  };

  // CDasherSettingsInterface methods

  ///
  /// Notification from CDasherInterface that a parameter has changed
  /// \param iParameter The parameter which has changed
  /// \todo Currently this is not connected to anything. We need to create a new GTK event and trigger a signal from here.
  ///

  virtual void HandleParameterNotification( int iParameter );

  // CDashEditbox methods

  ///
  /// Notification of events other than parameters having changed.
  /// \param pEvent Pointer to the event.
  ///

  void HandleEvent( CEvent *pEvent );

  // The following functions are obsolete and will soon be deleted from the interface

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void write_to_file() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void get_new_context(std::string& str, int max) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void output( const std::string &strText ) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void outputcontrol(void* pointer, int data, int type) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void deletetext( const std::string &strText ) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void Clear() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void SetEncoding(Opts::FileEncodingFormats Encoding) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void SetFont(std::string Name, long Size) {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void Cut() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void Copy() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void Paste() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void CopyAll() {};

  ///
  /// \deprecated Obsolete, soon to be removed from interface
  ///

  void SelectAll() {};

 private:

  ///
  /// The CDasherInterface wrapped in the control
  ///

  CDasherInterface *m_pInterface;

  GtkWidget *m_pVBox;
  GtkWidget *m_pCanvas;
  GtkWidget *m_pSpeedHScale;
  GtkWidget *m_pSpeedFrame;

  ///
  /// Abstracted input device object.
  ///

  CDasherMouseInput *m_pMouseInput;

  ///
  /// Cache of Pango layouts
  ///

  CPangoCache *m_pPangoCache;

  ///
  /// The CCanvas object
  ///

  CCanvas *m_pScreen;

  ///
  /// The GObject which is wrapping this class
  ///

  GtkDasherControl *m_pDasherControl;

};

#endif