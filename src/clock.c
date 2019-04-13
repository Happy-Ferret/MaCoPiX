//  MaCoPiX = Mascot Construnctive Pilot for X
//                                (ActX / Gtk+ Evolution)
//
//
//     clock.c
//     Creating and Drawing Panel Clock Windows
//
//                            Copyright 2002-2008  K.Chimari
//                                     http://rosegray.sakura.ne.jp/
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
//

#include "main.h"

GdkBitmap *mask_clk[2]={NULL,NULL};;

// from gui.c

void DrawPanelClock();
void DrawPanelClock2();

//GtkWidget * make_clock(typMascot *mascot){
void  make_clock(typMascot *mascot){
  mascot->clock_main = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_accept_focus(GTK_WINDOW(mascot->clock_main),FALSE);
  gtk_widget_set_app_paintable(mascot->clock_main, TRUE);
  gtk_widget_set_events(GTK_WIDGET (mascot->clock_main), 
			GDK_FOCUS_CHANGE_MASK | 
			GDK_BUTTON_MOTION_MASK | 
			GDK_BUTTON_RELEASE_MASK | 
			GDK_BUTTON_PRESS_MASK | 
			GDK_EXPOSURE_MASK);
  gtk_widget_realize(mascot->clock_main);

  gdk_window_set_decorations(gtk_widget_get_window(mascot->clock_main), 0);
#ifndef USE_WIN32
  /* gdk_window_set_override_redirect is not implemented (for warning) */
  gdk_window_set_override_redirect(gtk_widget_get_window(mascot->clock_main),TRUE);
#endif
  my_signal_connect(mascot->clock_main, "configure_event",
  		    dw_configure_clk, (gpointer)mascot);
  my_signal_connect(mascot->clock_main, "expose_event",
  		    dw_expose_clk, (gpointer)mascot);
  my_signal_connect(mascot->clock_main, "button_press_event",
  		    clk_drag_begin, (gpointer)mascot);
  my_signal_connect(mascot->clock_main, "button_release_event",
  		    clk_drag_end, (gpointer)mascot);
  my_signal_connect(mascot->clock_main, "motion_notify_event",
  		    clk_window_motion, (gpointer)mascot);

  gdk_window_set_cursor(gtk_widget_get_window(mascot->clock_main),mascot->cursor.clk);

  gtk_window_resize (GTK_WINDOW(mascot->clock_main), 1, 1);
  dw_configure_clk(mascot->clock_main, "configure_event",(gpointer)mascot);
}


#ifdef USE_WIN32
void make_clock_fg(typMascot *mascot){

  mascot->clock_fg = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_accept_focus(GTK_WINDOW(mascot->clock_fg),FALSE);
  gtk_widget_set_app_paintable(mascot->clock_fg, TRUE);
  gtk_widget_set_events(GTK_WIDGET (mascot->clock_fg), 
			GDK_FOCUS_CHANGE_MASK | 
			GDK_BUTTON_MOTION_MASK | 
			GDK_BUTTON_RELEASE_MASK | 
			GDK_BUTTON_PRESS_MASK | 
			GDK_EXPOSURE_MASK);
  gtk_widget_realize(mascot->clock_fg);
  gdk_window_set_decorations(gtk_widget_get_window(mascot->clock_fg), 0);


  my_signal_connect(mascot->clock_fg, "configure_event",
  		    dw_configure_clk, (gpointer)mascot);
  my_signal_connect(mascot->clock_fg, "expose_event",
  		    dw_expose_clk, (gpointer)mascot);
  my_signal_connect(mascot->clock_fg, "button_press_event",
		    clk_drag_begin, (gpointer)mascot);
  my_signal_connect(mascot->clock_fg, "button_release_event",
		    clk_drag_end, (gpointer)mascot);
  my_signal_connect(mascot->clock_fg, "motion_notify_event",
		    clk_window_motion, (gpointer)mascot);

  gdk_window_resize (gtk_widget_get_window(mascot->clock_fg), 1, 1);
  dw_configure_clk(mascot->clock_fg, "configure_event",(gpointer)mascot);
}
#endif


// �ѥͥ���פ�pixmap buffer�ؤ�����
void DrawPanelClock(typMascot *mascot)
{
  gint clk_width,clk_height;
  gint work_page;
  PangoLayout *pango_text;
  gint new_w, new_h;
  GdkGCValues gcv;

  work_page=mascot->clk_page;
  work_page^=1;

  pango_text=gtk_widget_create_pango_layout(mascot->clock_main,
					    mascot->digit);
  pango_layout_get_pixel_size(pango_text,&clk_width,&clk_height);


  switch(mascot->clktype){
  case CLOCK_TYPE_12S:
  case CLOCK_TYPE_12M:
    clk_width+=clk_height/3;
    break;
  }

  
  if (pixmap_clk[work_page]) {
    g_object_unref(G_OBJECT(pixmap_clk[work_page]));
  } 
  
  new_w=clk_width+(mascot->clktext_x+mascot->wclkbd)*2;
  new_h=clk_height+(mascot->clktext_y+mascot->wclkbd)*2;

  pixmap_clk[work_page] = gdk_pixmap_new(gtk_widget_get_window(mascot->clock_main),
					 new_w,
					 new_h,
					 -1);
  
  if(mascot->flag_clkrd){
    if (mask_clk[work_page]) {
      g_object_unref(G_OBJECT(mask_clk[work_page]));
    } 
    
    mask_clk[work_page] = gdk_pixmap_new(gtk_widget_get_window(mascot->clock_main),
					 new_w,new_h,1); // Depth =1 (Bitmap)
    
    if(mascot->gc_clkmask[work_page]){
      gdk_gc_unref(mascot->gc_clkmask[work_page]);
    }
    mascot->gc_clkmask[work_page]=gdk_gc_new(mask_clk[work_page]);
    // ���ä���ޥ�����foreground�ǥ��ꥢ����
    gdk_draw_rectangle(mask_clk[work_page],mascot->gc_clkmask[work_page],
		       TRUE,
		       0,0,
		       new_w,new_h);
    // �ޥ�����foreground������ʬ���ä����Τ�, �Ĥ�������ʬ��
    // background�������褹��
    gdk_gc_get_values(mascot->gc_clkmask[work_page], &gcv);
    gdk_gc_set_foreground(mascot->gc_clkmask[work_page], &gcv.background);
    gdk_gc_set_background(mascot->gc_clkmask[work_page], &gcv.foreground);


    // **** pixmap�ؤ�BUFFERING
    // Mask (Border��Ʊ��)
#ifdef USE_WIN32
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 0,0,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 (new_w+2)-mascot->clktext_x*2-1,0,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 0,(new_h+2)-mascot->clktext_y*2-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 (new_w+2)-mascot->clktext_x*2-1,
		 (new_h+2)-mascot->clktext_y*2-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
#else
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 0,0,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760,5760);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 new_w-mascot->clktext_x*2-1,0,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 0,new_h-mascot->clktext_y*2-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760*2,5760);
    gdk_draw_arc(mask_clk[work_page],mascot->gc_clkmask[work_page],
		 TRUE,
		 new_w-mascot->clktext_x*2-1,new_h-mascot->clktext_y*2-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760*3,5760);
#endif
    gdk_draw_rectangle(mask_clk[work_page],mascot->gc_clkmask[work_page],
		       TRUE,
		       0,mascot->clktext_y,
		       new_w,new_h-mascot->clktext_y*2);
    gdk_draw_rectangle(mask_clk[work_page],mascot->gc_clkmask[work_page],
		       TRUE,
		       mascot->clktext_x,0,
		       new_w-mascot->clktext_x*2,new_h);
    
    
    
    // border
    if(mascot->wclkbd>0){
#ifdef USE_WIN32
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   0,0,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   0,5760*4);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   (new_w+2)-mascot->clktext_x*2-1,0,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   0,5760*4);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   0,(new_h+2)-mascot->clktext_y*2-1,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   0,5760*4);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   (new_w+2)-mascot->clktext_x*2-1,(new_h+2)-mascot->clktext_y*2-1,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   0,5760*4);
#else
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   0,0,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   5760,5760);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   new_w-mascot->clktext_x*2-1,0,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   0,5760);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   0,new_h-mascot->clktext_y*2-1,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   5760*2,5760);
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		   TRUE,
		   new_w-mascot->clktext_x*2-1,(new_h+2)-mascot->clktext_y*2-1,
		   mascot->clktext_x*2,mascot->clktext_y*2,
		   5760*3,5760);
#endif
      gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
			 TRUE,
			 0,mascot->clktext_y,
			 new_w,new_h-mascot->clktext_y*2);
      gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
			 TRUE,
			 mascot->clktext_x,0,
			 new_w-mascot->clktext_x*2,new_h);
      
      
    }
    
    // background
#ifdef USE_WIN32
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 mascot->wclkbd,mascot->wclkbd,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 (new_w+2)-mascot->clktext_x*2-mascot->wclkbd-1,
		 mascot->wclkbd,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 mascot->wclkbd,
		 (new_h+2)-mascot->clktext_y*2-mascot->wclkbd-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 (new_w+2)-mascot->clktext_x*2-mascot->wclkbd-1,
		 (new_h+2)-mascot->clktext_y*2-mascot->wclkbd-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760*4);
#else
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 mascot->wclkbd,mascot->wclkbd,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760,5760);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 new_w-mascot->clktext_x*2-mascot->wclkbd-1,
		 mascot->wclkbd,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 0,5760);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 mascot->wclkbd,
		 new_h-mascot->clktext_y*2-mascot->wclkbd-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760*2,5760);
    gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		 TRUE,
		 new_w-mascot->clktext_x*2-mascot->wclkbd-1,
		 new_h-mascot->clktext_y*2-mascot->wclkbd-1,
		 mascot->clktext_x*2,mascot->clktext_y*2,
		 5760*3,5760);
#endif
    gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		       TRUE,
		       mascot->wclkbd,
		       mascot->wclkbd+mascot->clktext_y,
		       new_w-mascot->wclkbd*2,
		       new_h-mascot->clktext_y*2-mascot->wclkbd*2);
    gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		       TRUE,
		       mascot->wclkbd+mascot->clktext_x,
		       mascot->wclkbd,
		       new_w-mascot->clktext_x*2-mascot->wclkbd*2,
		       new_h-mascot->wclkbd*2);
    
    
  }
  else{
    
   // **** pixmap�ؤ�BUFFERING
   // border
    gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbd[work_page],
		       TRUE,
		       0,0,
		       new_w,
		       new_h);
    // background
    gdk_draw_rectangle(pixmap_clk[work_page],mascot->gc_clkbg[work_page],
		       TRUE,
		       mascot->wclkbd,mascot->wclkbd,
		       new_w-mascot->wclkbd*2,
		       new_h-mascot->wclkbd*2);
  }
  
  
  
  // digital clock
  if(mascot->flag_clksd){
    gdk_draw_layout(pixmap_clk[work_page],
		    mascot->gc_clksd[work_page],
		    mascot->clktext_x+mascot->wclkbd+mascot->clksd_x,
		    mascot->clktext_y+mascot->wclkbd+mascot->clksd_y,
		    pango_text);
  }

  gdk_draw_layout(pixmap_clk[work_page],
		  mascot->gc_clk[work_page],
		  mascot->clktext_x+mascot->wclkbd,
		  mascot->clktext_y+mascot->wclkbd,
		  pango_text);
  
  switch(mascot->clktype){
  case CLOCK_TYPE_12S:
  case CLOCK_TYPE_12M:
    // digital clock
    if(mascot->flag_clksd){
      if(mascot->clk_pm){
	gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clksd[work_page],
		     TRUE,
		     clk_width+mascot->clktext_x+mascot->wclkbd+mascot->clksd_x-clk_height/3,
		     mascot->clktext_y+mascot->wclkbd+mascot->clksd_y+clk_height*2/3,
		     clk_height/3,clk_height/3,
		     0,(360*64));
      }
      else{
	gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clksd[work_page],
		     TRUE,
		     clk_width+mascot->clktext_x+mascot->wclkbd+mascot->clksd_x-clk_height/3,
		     mascot->clktext_y+mascot->wclkbd+mascot->clksd_y,
		     clk_height/3,clk_height/3,
		     0,(360*64));
      }
    }
    
    if(mascot->clk_pm){
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clk[work_page],
		   TRUE,
		   clk_width+mascot->clktext_x+mascot->wclkbd-clk_height/3,
		   mascot->clktext_y+mascot->wclkbd+clk_height*2/3,
		   clk_height/3,clk_height/3,
		   0,(360*64));
    }
    else{
      gdk_draw_arc(pixmap_clk[work_page],mascot->gc_clk[work_page],
		   TRUE,
		   clk_width+mascot->clktext_x+mascot->wclkbd-clk_height/3,
		   mascot->clktext_y+mascot->wclkbd,
		   clk_height/3,clk_height/3,
		   0,(360*64));
    }
    
    break;
  }
  
  mascot->clk_page=work_page;
  gdk_flush();
  
#ifdef USE_WIN32
  if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
    gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_fg),
			       pixmap_clk[mascot->clk_page],
			       FALSE);
  }
#endif
  gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_main),
			     pixmap_clk[mascot->clk_page],
			     FALSE);
  
  
#ifdef USE_WIN32
  if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
    gdk_window_resize (gtk_widget_get_window(mascot->clock_fg),new_w,new_h);
  }
#endif
  gdk_window_resize (gtk_widget_get_window(mascot->clock_main), new_w, new_h);

  if(mascot->flag_clkrd){
#ifdef USE_WIN32
    if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
      gdk_window_shape_combine_mask( gtk_widget_get_window(mascot->clock_fg),
				     mask_clk[mascot->clk_page],
				     0,0);
    }
#endif
    
    gdk_window_shape_combine_mask( gtk_widget_get_window(mascot->clock_main),
				   mask_clk[mascot->clk_page],
				   0,0);
  }

#ifdef USE_WIN32
  gdk_draw_drawable(gtk_widget_get_window(mascot->clock_fg),
		    mascot->clock_fg->style->fg_gc[GTK_WIDGET_STATE(mascot->clock_main)],
		    pixmap_clk[mascot->clk_page],
		    0,0,0,0,
		    new_w,
		    new_h);
#endif

  gdk_draw_drawable(gtk_widget_get_window(mascot->clock_main),
		    mascot->clock_main->style->fg_gc[GTK_WIDGET_STATE(mascot->clock_main)],
		    pixmap_clk[mascot->clk_page],
		    0,0,0,0,
		    new_w,
		    new_h);

  g_object_unref(G_OBJECT(pango_text));
  
  while (my_main_iteration(FALSE));
  gdk_flush();
  
}


// �ѥͥ���פ�pixmap buffer�ؤ�����
void DrawPanelClock2(typMascot *mascot)
{
  gint clk_width,clk_height;
  gint work_page;
  gint new_w, new_h;
  cairo_t *cr;
  cairo_t *cr_mask;
  gdouble M_PI=3.14159265;
  gdouble dx,dy;
  cairo_text_extents_t extents;
  PangoLayout *pango_text;
  gdouble ampmsize=0;
  gboolean shape_flag=FALSE;


  work_page=mascot->clk_page;
  work_page^=1;
  if(mascot->flag_clkrd){
    if(mascot->flag_composite==COMPOSITE_FALSE){
      shape_flag=TRUE;
    }
    else if((!mascot->force_composite)&&(mascot->flag_composite==COMPOSITE_UNKNOWN)){
      shape_flag=TRUE;
    }
  }

#ifdef __PANGOCAIRO_H__
  pango_text=gtk_widget_create_pango_layout(mascot->clock_main,
					    mascot->digit);
  pango_layout_get_pixel_size(pango_text,&clk_width,&clk_height);
#endif

  cr = gdk_cairo_create(gtk_widget_get_window(mascot->clock_main));
 
  cairo_select_font_face (cr, 
			  mascot->fontclk_pc.family,
			  mascot->fontclk_pc.slant,
			  mascot->fontclk_pc.weight);
  cairo_set_font_size (cr, 
		       mascot->fontclk_pc.pointsize*GetCurrentResolution()/72.0);
#ifndef __PANGOCAIRO_H__
  cairo_text_extents (cr, mascot->digit, &extents);
  clk_width=(gint)(extents.x_advance);
  clk_height=(gint)(extents.height);
#endif

  switch(mascot->clktype){
  case CLOCK_TYPE_12S:
  case CLOCK_TYPE_12M:
    cairo_text_extents (cr, "AM", &extents);
    ampmsize=(gint)(extents.x_advance*CLOCK_AMPM_RATIO);
    clk_width+=ampmsize+3;
    break;
  }

  cairo_destroy(cr);

  
  if (pixmap_clk[work_page]) {
    g_object_unref(G_OBJECT(pixmap_clk[work_page]));
  } 
  
  new_w=clk_width+(mascot->clktext_x+mascot->wclkbd)*2;
  new_h=clk_height+(mascot->clktext_y+mascot->wclkbd)*2;

  pixmap_clk[work_page] = gdk_pixmap_new(gtk_widget_get_window(mascot->clock_main),
					 new_w,
					 new_h,
					 -1);

  if(shape_flag){
    if (mask_clk[work_page]) {
      g_object_unref(G_OBJECT(mask_clk[work_page]));
    } 
    
    mask_clk[work_page] = gdk_pixmap_new(gtk_widget_get_window(mascot->clock_main),
					 new_w,new_h,1); // Depth =1 (Bitmap)
    
    cr_mask = gdk_cairo_create(mask_clk[work_page]);
    cairo_set_operator (cr_mask, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr_mask);
    cairo_set_operator (cr_mask, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb (cr_mask, 1, 1, 1); // opaque white
  }
  
  
  cr = gdk_cairo_create(pixmap_clk[work_page]);
  
  if((mascot->flag_composite==COMPOSITE_TRUE)
     ||((mascot->force_composite)&&(mascot->flag_composite==COMPOSITE_UNKNOWN)))
    cairo_set_source_rgba (cr, 1, 1, 1, 0);
  else
    cairo_set_source_rgb (cr, 1, 1, 1); // opaque white
  
  // draw background

  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  cairo_select_font_face (cr, 
			  mascot->fontclk_pc.family,
			  mascot->fontclk_pc.slant,
			  mascot->fontclk_pc.weight);
  cairo_set_font_size (cr, 
		       mascot->fontclk_pc.pointsize*GetCurrentResolution()/72.0);

  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

#ifdef USE_WIN32  
  //BG should be opaque to BG only translucency for Win32
  cairo_set_source_rgba (cr, 
			 (gdouble)mascot->colclkbg->red/0xFFFF,
			 (gdouble)mascot->colclkbg->green/0xFFFF,
			 (gdouble)mascot->colclkbg->blue/0xFFFF,
			 1); /* opaque BG */
  cairo_rectangle(cr, 0, 0, new_w, new_h);
  cairo_fill(cr);
#else
  cairo_set_source_rgba (cr, 
			 (gdouble)mascot->colclkbg->red/0xFFFF,
			 (gdouble)mascot->colclkbg->green/0xFFFF,
			 (gdouble)mascot->colclkbg->blue/0xFFFF,
			 (gdouble)mascot->alpclkbg/0xFFFF); /* transparent */
  if(shape_flag){
    cairo_rectangle(cr, 0, 0, new_w, new_h);
    cairo_fill(cr);
  }
#endif

  if(mascot->flag_clkrd){
    dx=(gdouble)mascot->clktext_x;
    dy=(gdouble)mascot->clktext_y;

    cairo_move_to(cr,0,dy);

    cairo_save (cr);
    cairo_translate (cr, dx, dy);
    cairo_scale (cr, dx, dy);
    cairo_arc (cr, 0, 0, 1., 180*(M_PI/180.), 270*(M_PI/180.));
    cairo_restore (cr);

    cairo_line_to(cr, new_w-dx, 0);

    cairo_save (cr);
    cairo_translate (cr, new_w-dx, dy);
    cairo_scale (cr, dx, dy);
    cairo_arc (cr, 0, 0, 1., 270*(M_PI/180.), 360*(M_PI/180.));
    cairo_restore (cr);

    cairo_line_to(cr, new_w, new_h-dy);

    cairo_save (cr);
    cairo_translate (cr, new_w-dx, new_h-dy);
    cairo_scale (cr, dx, dy);
    cairo_arc (cr, 0, 0, 1., 0*(M_PI/180.), 90*(M_PI/180.));
    cairo_restore (cr);

    cairo_line_to(cr, dx,  new_h);

    cairo_save (cr);
    cairo_translate (cr, dx, new_h-dy);
    cairo_scale (cr, dx, dy);
    cairo_arc (cr, 0, 0, 1., 90*(M_PI/180.), 180*(M_PI/180.));
    cairo_restore (cr);

    cairo_line_to(cr, 0,  dy);
  }
  else{
    cairo_rectangle(cr, 0, 0, new_w, new_h);
  }

  if(shape_flag){
    cairo_append_path(cr_mask,cairo_copy_path(cr));
    cairo_fill(cr_mask);
    // If having a mask, never clip
    // Anti-alias of cairo could cause discoloration
  }
  else{
    cairo_clip_preserve (cr);
    cairo_fill_preserve(cr);
  }


  if(mascot->wclkbd>0){
    cairo_set_source_rgba (cr, 
			   (gdouble)mascot->colclkbd->red/0xFFFF,
			   (gdouble)mascot->colclkbd->green/0xFFFF,
			   (gdouble)mascot->colclkbd->blue/0xFFFF,
			   (gdouble)mascot->alpclkbd/0xFFFF); /* transparent */
    cairo_set_line_width(cr,(gdouble)mascot->wclkbd *2);
    cairo_stroke(cr);
  }

  
    
  cairo_set_line_width(cr,1.0);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    
    
  if(mascot->flag_clksd){
    cairo_set_source_rgba (cr, 
			   (gdouble)mascot->colclksd->red/0xFFFF,
			   (gdouble)mascot->colclksd->green/0xFFFF,
			   (gdouble)mascot->colclksd->blue/0xFFFF,
			   (gdouble)mascot->alpclksd/0xFFFF); /* transparent */
#ifdef __PANGOCAIRO_H__
    cairo_move_to(cr,
		  mascot->wclkbd+mascot->clktext_x+mascot->clksd_x,
		  mascot->wclkbd+mascot->clktext_y+mascot->clksd_y);
    pango_cairo_show_layout(cr,pango_text);
#else
    cairo_text_extents (cr, mascot->digit, &extents);
    cairo_move_to(cr,
		  mascot->wclkbd+mascot->clktext_x+mascot->clksd_x,
		  -extents.y_bearing+mascot->wclkbd+mascot->clktext_y+mascot->clksd_y);
    cairo_show_text(cr,mascot->digit);
#endif

    
    switch(mascot->clktype){
    case CLOCK_TYPE_12S:
    case CLOCK_TYPE_12M:
      cairo_move_to(cr, 
		    new_w-ampmsize-mascot->wclkbd-mascot->clktext_x+mascot->clksd_x-1,
		    new_h-mascot->wclkbd-mascot->clktext_y+mascot->clksd_y-1);
      cairo_save(cr);
      cairo_scale (cr, CLOCK_AMPM_RATIO, CLOCK_AMPM_RATIO);
		    
      // digital clock
      if(mascot->clk_pm){
	cairo_show_text(cr,"PM");
      }
      else{
	cairo_show_text(cr,"AM");
      }
      cairo_restore(cr);
      
      break;
    }
  }
  
  
  cairo_set_source_rgba (cr, 
			 (gdouble)mascot->colclk->red/0xFFFF,
			 (gdouble)mascot->colclk->green/0xFFFF,
			 (gdouble)mascot->colclk->blue/0xFFFF,
			 (gdouble)mascot->alpclk/0xFFFF); /* transparent */
#ifdef __PANGOCAIRO_H__
  cairo_move_to(cr,
		mascot->wclkbd+mascot->clktext_x,
		mascot->wclkbd+mascot->clktext_y);
  pango_cairo_show_layout(cr,pango_text);
#else
  cairo_text_extents (cr, mascot->digit, &extents);

  cairo_move_to(cr,
		mascot->wclkbd+mascot->clktext_x,
		-extents.y_bearing+mascot->wclkbd+mascot->clktext_y);
  cairo_show_text(cr,mascot->digit);
#endif
  
    
  switch(mascot->clktype){
  case CLOCK_TYPE_12S:
  case CLOCK_TYPE_12M:
    cairo_move_to(cr, 
		  new_w-ampmsize-mascot->wclkbd-mascot->clktext_x-1,
		  new_h-mascot->wclkbd-mascot->clktext_y-1);
    cairo_save(cr);
    cairo_scale (cr, CLOCK_AMPM_RATIO, CLOCK_AMPM_RATIO);
    // digital clock
    if(mascot->clk_pm){
      cairo_show_text(cr,"PM");
    }
    else{
      cairo_show_text(cr,"AM");
    }
    cairo_restore(cr);
    
    break;
  }

    
  mascot->clk_page=work_page;
  gdk_flush();
  
#ifdef USE_WIN32
  if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
    gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_fg),
			       pixmap_clk[mascot->clk_page],
			       FALSE);
  }
#endif
  gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_main),
  			     pixmap_clk[mascot->clk_page],
  			     FALSE);
  
#ifdef USE_WIN32
  if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
    gdk_window_resize (gtk_widget_get_window(mascot->clock_fg), new_w, new_h);
  }
#endif
  gdk_window_resize (gtk_widget_get_window(mascot->clock_main), new_w, new_h);

  if(shape_flag){
#ifdef USE_WIN32
    if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
      gdk_window_shape_combine_mask( gtk_widget_get_window(mascot->clock_fg),
				     mask_clk[mascot->clk_page],
				     0,0);
    }
#endif

    gdk_window_shape_combine_mask( gtk_widget_get_window(mascot->clock_main),
				   mask_clk[mascot->clk_page],
				   0,0);
    cairo_destroy(cr_mask);
  }

#ifdef USE_WIN32
  gdk_draw_drawable(gtk_widget_get_window(mascot->clock_fg),
		    mascot->clock_fg->style->fg_gc[GTK_WIDGET_STATE(mascot->clock_main)],
		    pixmap_clk[mascot->clk_page],
		    0,0,0,0,
		    new_w,
		    new_h);
#endif
  gdk_draw_drawable(gtk_widget_get_window(mascot->clock_main),
		    mascot->clock_main->style->fg_gc[GTK_WIDGET_STATE(mascot->clock_main)],
		    pixmap_clk[mascot->clk_page],
		    0,0,0,0,
		    new_w,
		    new_h);
  
  cairo_destroy(cr);
  
#ifdef __PANGOCAIRO_H__
  g_object_unref(G_OBJECT(pango_text));
#endif
  
  while(my_main_iteration(FALSE));
  gdk_flush();
  
}

void DrawPanelClock0(typMascot *mascot){
  //  if(((mascot->flag_clk_cairo)&&(mascot->flag_composite!=COMPOSITE_FALSE))
  //     ||(!mascot->flag_clkrd))
  if(mascot->flag_clk_cairo)
    DrawPanelClock2(mascot);
  else
    DrawPanelClock(mascot);
}
