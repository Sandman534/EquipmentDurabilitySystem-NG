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
	private var reverse:Boolean = false;
	private var layout:Number = 0;

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
		}
	}
	
	private function onLoadInit(a_icon:MovieClip):Void {
		a_icon.gotoAndStop(1);
	}
	
	public function Setup(a_x:Number, a_y:Number, a_scale:Number, a_layout:Number, a_reverse:Boolean, a_shout:Boolean, a_armor:Boolean, a_weapon:Boolean, a_health:Boolean):Void {	
		scale = a_scale;
		layout = (a_layout == 1) ? 1 : 0;
		reverse = a_reverse;

		// Base Values
		var scaleFactor:Number = a_scale / 100;
		var baseX:Number = Stage.width * a_x;
		var baseY:Number = Stage.height * a_y;
		var spacing:Number = 30 * scaleFactor;
		var textOffset:Number = 15 * scaleFactor;
		var iconXAdjustment:Number = -6 * scaleFactor;
		var iconYAdjustment:Number = 2 * scaleFactor;

		// Horizontal Layout Sensitivities
		var horizontalTextAdjustment:Number = 10 * scaleFactor;
		var horizontalValueAdjustment:Number = 10 * scaleFactor;
		var horizontalCenterAdjustment:Number = 4 * scaleFactor;

		// Vertical Layout Sensitivities
		var verticalTextAdjustment:Number = -2 * scaleFactor;
		var verticalValueAdjustment:Number = 2 * scaleFactor;
		var verticalTextXAdjustment:Number = 4 * scaleFactor;
		var verticalValueXAdjustment:Number = 4 * scaleFactor;

		// Loop through all holders
		for (var i:Number = 0; i < 7; i++) {
			var iconX:Number = baseX;
			var iconY:Number = baseY;
			if (layout == 1) {
				iconX += i * spacing;
			} else {
				iconY += i * spacing;
			}

			// Icon X and Y
			iconHolders[i]._x = iconX + iconXAdjustment;
			iconHolders[i]._y = iconY + iconYAdjustment;

			textHolders[i]._xscale = textHolders[i]._yscale = a_scale;
			valueHolders[i]._xscale = valueHolders[i]._yscale = a_scale;

			textHolders[i].ins_.autoSize = "left";
			valueHolders[i].ins_.autoSize = "left";

			if (layout == 1) {
				textHolders[i]._x = iconX + horizontalCenterAdjustment;
				valueHolders[i]._x = iconX + horizontalCenterAdjustment;
				textHolders[i]._y = iconY + (reverse ? textOffset : -textOffset) + horizontalTextAdjustment;
				valueHolders[i]._y = iconY + (reverse ? -textOffset : textOffset) + horizontalValueAdjustment;
				textHolders[i]._rotation = reverse ? 90 : -90;
			} else {
				textHolders[i]._rotation = 0;
				textHolders[i]._y = iconY + verticalTextAdjustment;
				valueHolders[i]._y = iconY + verticalValueAdjustment;
				if (reverse) {
					// Name to the left, health to the right.
					textHolders[i]._x = iconX - textOffset + verticalTextXAdjustment;
					valueHolders[i]._x = iconX + textOffset + verticalValueXAdjustment;
				} else {
					// Name to the right, health to the left.
					textHolders[i]._x = iconX + textOffset + verticalTextXAdjustment;
					valueHolders[i]._x = iconX - textOffset + verticalValueXAdjustment;
				}
			}

			// Shout Name Visibility
			if (i == 6) {
				iconHolders[i]._visible = a_shout;
				textHolders[i]._visible = a_shout;
				valueHolders[i]._visible = a_shout && a_health;
			} else {
				valueHolders[i]._visible = a_health;
			}

			// Weapon Name Visibility
			if (i >= 4 && i <= 5) {
				textHolders[i]._visible = a_weapon;
			}

			// Armor Name Visibility
			if (i >= 0 && i <= 3) {
				textHolders[i]._visible = a_armor;
			}
		}
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
		var itemData = new Array(a_data,b_data,c_data,d_data,e_data,f_data,g_data);
	
		if (a_icons.length != 7 || a_texts.length != 7 || a_values.length != 7 || a_colors.length != 7) {
			return;
		}		
				
		for (var i:Number = 0; i < 7; i++) {	
			//==============================================================
			// Icon Preparation
			//==============================================================
			var newSource = DEFAULT_ICON_SOURCE;
			var newIcon = a_icons[i];
			var newColor = a_colors[i];
			
			// If we have item Data, otherwise populate with defaults
			if(itemData[i]) {
				// Call i4 if it is installed
				skse.plugins.InventoryInjector.ProcessEntry(itemData[i]);
			
				newSource = itemData[i].iconSource ? itemData[i].iconSource : DEFAULT_ICON_SOURCE;
				newIcon = itemData[i].iconLabel ? itemData[i].iconLabel : a_icons[i];
			}
			
			// Fix the filename
			if (widgetHolder._url.indexOf("exported") != -1 || widgetHolder._url.indexOf("Exported") != -1) {
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
			if(typeof(newColor) == "number") {
				colorTransform.rgb = newColor;
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

			if (layout == 1) {
				textHolders[i].ins_._x = 0;
				textHolders[i].ins_._y = -textHolders[i].ins_._height / 2;
			} else if (reverse) {
				// Holder marks the right edge.
				textHolders[i].ins_._x = -textHolders[i].ins_._width;
				textHolders[i].ins_._y = 0;
			} else {
				// Holder marks the left edge.
				textHolders[i].ins_._x = 0;
				textHolders[i].ins_._y = 0;
			}

			//==============================================================
			// Health Value
			//==============================================================			
			if (a_values[i] == -1) {
				valueHolders[i].ins_.text = "";
			} else {
				valueHolders[i].ins_.textColor = a_colors[i];
				valueHolders[i].ins_.text = a_values[i];
			}

			if (layout == 1) {
				valueHolders[i].ins_._x = -valueHolders[i].ins_._width / 2;
				valueHolders[i].ins_._y = reverse ? -valueHolders[i].ins_._height : 0;
			} else if (reverse) {
				valueHolders[i].ins_._x = 0;
				valueHolders[i].ins_._y = 0;
			} else {
				valueHolders[i].ins_._x = -valueHolders[i].ins_._width;
				valueHolders[i].ins_._y = 0;
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
