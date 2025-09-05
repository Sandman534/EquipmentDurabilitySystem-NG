import flash.geom.ColorTransform;
import flash.geom.Transform;

class DurabilityWidget {
	private static var DEFAULT_ICON_SOURCE = "skyui/icons_item_psychosteve.swf";

	private var widgetHolder:MovieClip;
	public var scale:Number = 100;
	
	public var iconHolders:Array;
	public var sourceHolders:Array;
	public var textHolders:Array;
	public var valueHolders:Array;
	
	private var iconLoader: MovieClipLoader;
	private var debugTF:TextField;

	// Constructor
	public function DurabilityWidget(swfRoot:MovieClip) {
		super();
		
		widgetHolder = swfRoot;
		// initDebugConsole();
		
		Stage.align = "TL";
		Stage.scaleMode = "noScale";
		
		// Array that holds each widget
		iconHolders = new Array();
		sourceHolders = new Array();
		textHolders = new Array();
		valueHolders = new Array();
		
		// Setup the icon loader
		iconLoader = new MovieClipLoader();
		iconLoader.addListener(this);
		
		// DebugLog("Setup Loader");
		
		// Store all the holders into 4 arrays
		for (var i:Number = 0; i < 7; i++) {
			var mc_icon = org.casalib.util.MovieClipUtil.createEmptyMovieClip(widgetHolder, "icon" + String(i), widgetHolder.getNextHighestDepth(), {_x:0, _y:0});
			iconLoader.loadClip(DEFAULT_ICON_SOURCE, mc_icon);
			mc_icon._visible = true;
			iconHolders.push(mc_icon);
			sourceHolders.push(undefined);
			
			var mc_name = widgetHolder.attachMovie("TextIns", "name" + String(i), widgetHolder.getNextHighestDepth());
			var name_format:TextFormat = mc_name.ins_.getNewTextFormat();
			name_format.font = "$EverywhereMediumFont";
			name_format.size = 20;
			mc_name.ins_.setNewTextFormat(name_format);
			mc_name.ins_.text = "";
			mc_name._visible = true;
			textHolders.push(mc_name);
			
			var mc_value = widgetHolder.attachMovie("TextIns", "value" + String(i), widgetHolder.getNextHighestDepth());
			var value_format:TextFormat = mc_value.ins_.getNewTextFormat();
			value_format.font = "$EverywhereMediumFont";
			value_format.size = 15;
			mc_value.ins_.setNewTextFormat(value_format);
			mc_value.ins_.text = "";
			mc_value._visible = true;
			valueHolders.push(mc_value);
			
			// DebugLog("Processed Icon 0" + i);
		}
	}
	
	private function onLoadInit(a_icon:MovieClip):Void {
		a_icon.gotoAndStop(1);
	}
	
	public function Setup(a_x:Number, a_y:Number, a_scale:Number, a_reverse:Boolean, a_shout:Boolean, a_armor:Boolean, a_weapon:Boolean, a_health:Boolean):Void {	
		scale = a_scale;
		
		// DebugLog("Reverse: " + a_reverse);
		// DebugLog("Shout: " + a_shout);
		// DebugLog("Armor Name: " + a_armor);
		// DebugLog("Weapon Name: " + a_weapon);
		// DebugLog("Health: " + a_health);
		
		for (var i:Number = 0; i < 7; i++) {
			iconHolders[i]._x = Stage.width * a_x - 4 * a_scale / 100 - 4;
			iconHolders[i]._y = Stage.height * a_y + i * 30 * a_scale / 100 + 4;
			
			textHolders[i]._y = Stage.height * a_y + i * 30 * a_scale / 100;
			textHolders[i]._xscale = textHolders[i]._yscale = a_scale;
			valueHolders[i]._y = Stage.height * a_y + (i * 30 + 5) * a_scale / 100;
			valueHolders[i]._xscale = valueHolders[i]._yscale = a_scale;
			
			if (a_reverse) {
				textHolders[i]._x = Stage.width * a_x - 15 * a_scale / 100;
				textHolders[i].ins_.autoSize = "right";
				valueHolders[i]._x = Stage.width * a_x + 15 * a_scale / 100;
				valueHolders[i].ins_.autoSize = "left";
			} else {
				textHolders[i]._x = Stage.width * a_x + 15 * a_scale / 100;
				textHolders[i].ins_.autoSize = "left";
				valueHolders[i]._x = Stage.width * a_x - 15 * a_scale / 100;
				valueHolders[i].ins_.autoSize = "right";
			}
			
			if (i == 6) {
				iconHolders[i]._visible = a_shout;
				textHolders[i]._visible = a_shout;
				valueHolders[i]._visible = a_shout;
			}
			
			if (i >= 4 && i <= 5) {
				textHolders[i]._visible = a_weapon;
			}
			
			if (i >= 0 && i <= 3) {
				textHolders[i]._visible = a_armor;
			}
			
			valueHolders[i]._visible = a_health;
		}
		
		// DebugLog("Finished Setup");
	}
	
	public function UpdateMenu(
		a_icon: String, a_text: String, a_value: Number, a_color: Number, a_data: Object, 
		b_icon: String, b_text: String, b_value: Number, b_color: Number, b_data: Object,
		c_icon: String, c_text: String, c_value: Number, c_color: Number, c_data: Object,
		d_icon: String, d_text: String, d_value: Number, d_color: Number, d_data: Object,
		e_icon: String, e_text: String, e_value: Number, e_color: Number, e_data: Object,
		f_icon: String, f_text: String, f_value: Number, f_color: Number, f_data: Object,
		g_icon: String, g_text: String, g_value: Number, g_color: Number, g_data: Object	
	):Void
	{
		var a_icons = new Array(a_icon,b_icon,c_icon,d_icon,e_icon,f_icon,g_icon);
      var a_texts = new Array(a_text,b_text,c_text,d_text,e_text,f_text,g_text);
      var a_values = new Array(a_value,b_value,c_value,d_value,e_value,f_value,g_value);
      var a_colors = new Array(a_color,b_color,c_color,d_color,e_color,f_color,g_color);
		 var a_data = new Array(a_data,b_data,c_data,d_data,e_data,f_data,g_data);
	
		// DebugLog("Start Processing Update");
		if (a_icons.length != 7 || a_texts.length != 7 || a_values.length != 7 || a_colors.length != 7) {
			// DebugLog("Something went wrong [" + a_icons.length + "] [" + a_texts.length + "] [" + a_values.length + "] [" + a_colors.length + "]");
			return;
		}		
				
		for (var i:Number = 0; i < 7; i++) {	
			//==============================================================
			// Icon Preparation
			//==============================================================
			var newSource = DEFAULT_ICON_SOURCE;
			var newIcon = a_icons[i];
					
			// If we have item Data, otherwise populate with defaults
			if(a_data[i]) {
				// Call i4 if it is installed
				skse.plugins.InventoryInjector.ProcessEntry(a_data[i]);
			
				newSource = a_data[i].iconSource ? a_data[i].iconSource : DEFAULT_ICON_SOURCE;
				newIcon = a_data[i].iconLabel ? a_data[i].iconLabel : a_icons[i];
			}
			
			// Fix the filename
			if (_url.indexOf("exported") != -1 || _url.indexOf("Exported") != -1) {
				newSource = "../" + newSource;
			}
			
			// If our source changed, update
			if(newSource != sourceHolders[i]) {
				sourceHolders[i] = newSource
				iconLoader.loadClip(sourceHolders[i], iconHolders[i]);
			}
			
			// Set Text Color
			var colorTransform = new ColorTransform();
			var transform = new Transform(MovieClip(iconHolders[i]));
			if(typeof(a_colors[i]) == "number") {
				colorTransform.rgb = a_colors[i];
			}
			transform.colorTransform = colorTransform;			
			
			// Set the icon and adjust it
			if (newIcon == "none") {
				iconHolders[i].gotoAndStop(1);
			} else {
				iconHolders[i].gotoAndStop(newIcon);
				iconHolders[i]._width = 20 * scale / 100;
				iconHolders[i]._height = 20 * scale / 100;
			}

			//==============================================================
			// Item Names
			//==============================================================				
			textHolders[i].ins_.textColor = a_colors[i];
			textHolders[i].ins_.text = a_texts[i];

			//==============================================================
			// Health Value
			//==============================================================			
			if (a_values[i] == -1) {
				valueHolders[i].ins_.text = "";
			} else {
				valueHolders[i].ins_.textColor = a_colors[i];
				valueHolders[i].ins_.text = a_values[i];
			}
		}
	}
	private function initDebugConsole():Void {
		var depth:Number = widgetHolder.getNextHighestDepth();

		debugTF = widgetHolder.createTextField("debugTF", depth, 10, 10, 400, 400);
		debugTF.multiline = true;
		debugTF.wordWrap = true;
		debugTF.background = true;
		debugTF.backgroundColor = 0x000000;
		debugTF.textColor = 0xFFFFFF;
		debugTF.border = true;
		
		var fmt:TextFormat = new TextFormat();
		fmt.font = "$EverywhereMediumFont";
		fmt.size = 20;

		debugTF.setNewTextFormat(fmt);
		debugTF.defaultTextFormat = fmt;
		
		debugTF.text = "[Debug Console Ready]\n";
	}
	
	// Append a message to the debug log
	public function DebugLog(msg:String):Void {
		if (!debugTF) return;

		debugTF.text += msg + "\n";
		debugTF.scroll = debugTF.maxscroll; // auto-scroll to bottom
	}
}
