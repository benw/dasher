#ifndef __canvas_h__
#define __canvas_h__

#include "PangoCache.h"

#include "../DasherCore/DasherScreen.h"
#include "../DasherCore/DasherTypes.h"
#include "../DasherCore/CustomColours.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>


using namespace Dasher;

/// CCanvas
///
/// Method definitions for CCanvas, implementing the CDasherScreen
/// interface.  Please think very carefully before implementing new
/// functionality in this class. Anything which isn't a 'drawing
/// primitive' should really not be here - higher level drawing
/// functions belong in CDasherView.

class CCanvas : public Dasher::CDasherScreen {

 public:

  /// 
  /// \param pCanvas The GTK drawing area used by the canvas
  /// \param pPangoCache A cache for precomputed Pango layouts
  ///

  CCanvas( GtkWidget *pCanvas, CPangoCache *pPangoCache );
  ~CCanvas();

  ///
  /// GTK signal handler for exposure of the canvas - cause a redraw to the screen from the buffer.
  ///

  void ExposeEvent( GdkEventExpose *pEvent );

  // CDasherScreen methods

  ///
  /// Set the font used to render the Dasher display
  /// \param Name The name of the font.
  /// \todo This needs to be reimplemented for 4.0
  ///  

  void SetFont(std::string Name) {};

  ///
  /// Set the font size for rendering
  /// \param fontsize The font size to use
  /// \todo This needs to be reimplemented for 4.0
  ///

  void SetFontSize(Dasher::Opts::FontSize fontsize) {};

  ///
  /// Get the current font size
  /// \deprecated To be removed before 4.0 release
  /// \todo We should not be relying on locally cached variables - check to see whether this is still used or not
  ///

  Dasher::Opts::FontSize GetFontSize() const { return Dasher::Opts::FontSize(1); };

  ///
  /// Return the physical extent of a given string being rendered at a given size.
  /// \param String The string to be rendered
  /// \param Width Pointer to a variable to be filled with the width
  /// \param Height Pointer to a variable to be filled with the height
  /// \param Size Size at which the string will be rendered (units?)
  ///

  void TextSize(const std::string &String, screenint* Width, screenint* Height, int Size) const;

  ///
  /// \deprecated To be removed before 4.0 release
  /// \bug There should be no reference to symbol data types in CCanvs
  ///

  void DrawString(symbol Character, screenint x1, screenint y1, int Size) const;

  ///
  /// Draw a text string
  /// \param String The string to be rendered
  /// \param x1 The x coordinate at which to draw the text (be more precise)
  /// \param y1 The y coordinate at which to draw the text (be more precise)
  /// \param Size The size at which to render the rectangle (units?)
  ///

  void DrawString(const std::string &String, screenint x1, screenint y1, int Size) const;

  ///
  /// Draw a rectangle
  /// \param x1 x coordiate of the top left corner
  /// \param y1 y coordiate of the top left corner
  /// \param x2 x coordiate of the bottom right corner
  /// \param y2 y coordiate of the bottom right corner
  /// \param Color Colour to draw the rectangle
  /// \param ColorScheme Which of the alternating colour schemes to use (be more precise)
  ///

  void DrawRectangle(screenint x1, screenint y1, screenint x2, screenint y2, int Color, Opts::ColorSchemes ColorScheme) const;

  ///
  /// Send a marker to indicate phases of the redraw process. This is
  /// done so that we can do tripple buffering to minimise the amount
  /// of costly font rendering which needs to be done. Marker 1
  /// indicates that start of a new frame. Marker 2 indicates that we
  /// are now drawing decoration (such as mouse cursors etc.) rather
  /// than tne background. Only marker 2 will be send while Dasher is
  /// paused.
  /// \param iMarker ID of the marker being sent.
  ///

  void SendMarker( int iMarker );

  /// 
  /// Draw a polyline
  /// \param Points Array of vertices
  /// \param Number Size of 'Points' array
  /// \bug This seems dumb - surely we can just call the coloured version with the colour set to black?
  ///

  void Polyline(point* Points, int Number) const;

  /// 
  /// Draw a coloured polyline
  /// \param Points Array of vertices
  /// \param Number Size of 'Points' array
  /// \param Colour Colour with which to draw the line
  ///

  void Polyline(point* Points, int Number, int Colour) const;

  /// 
  /// Like polyline, but fill the shape
  /// \todo See comments for DrawPolygon
  ///

  void Polygon(point* Points, int Number, int Colour) const;

  ///
  /// \todo Not implemented
  /// \todo One of these two routines must be redundant - find out which and kill the other
  ///
  
  void DrawPolygon(point* Points, int Number, int Color, Opts::ColorSchemes ColorScheme) const { 
    // FIXME - not implemented 
  };

  /// 
  /// Blank the diplay
  ///

  void Blank() const;

  /// 
  /// Marks the end of the display process - at this point the offscreen buffer is copied onscreen.
  ///

  void Display();

  ///
  /// Update the colour definitions
  /// \param Colours New colours to use
  ///

  void SetColourScheme(const CCustomColours *Colours);

 private:
  
  ///
  /// The GTK drawing area for the canvas
  ///

  GtkWidget *m_pCanvas;

  ///
  /// The offscreen buffer containing the 'background'
  ///

  GdkPixmap *m_pDisplayBuffer;

  /// 
  /// The offscreen buffer containing the full display. This is
  /// constructed by first copying the display buffer across and then
  /// drawing decorations such as the mouse cursor on top.
  ///

  GdkPixmap *m_pDecorationBuffer;

  ///
  /// The onscreen buffer - copied onscreen whenever an expose event occurs.
  ///

  GdkPixmap *m_pOnscreenBuffer;
  
  ///
  /// Pointer to which of the offscreen buffers is currently active.
  ///

  GdkPixmap *m_pOffscreenBuffer;
  
  /// 
  /// The Pango cache - used to store pre-computed pango layouts as
  /// they are costly to regenerate every time they are needed.
  ///

  CPangoCache *m_pPangoCache;

  /// 
  /// Array of colours
  ///

  GdkColor *colours;
  
  /// 
  /// Canvas width
  ///

  int m_iWidth;

  ///
  /// Canvas height
  ///

  int m_iHeight;

  PangoRectangle *m_pPangoInk;
  PangoRectangle *m_pPangoLogical;

  ///
  /// The signal handler ID for the expose callback - stored so it can
  /// be disconnected when the CCanvas object is destroyed (which will
  /// happen whenever the canvas is resized).
  ///

  gulong lSignalHandler;

};

#endif