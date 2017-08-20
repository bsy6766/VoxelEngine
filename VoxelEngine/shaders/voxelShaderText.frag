#version 430

uniform sampler2D tex;
uniform bool outlined;
uniform int outlineSize;

//uniform float fontTextureWidth;
//uniform float fontTextureHeight;

in vec4 vertColor;
in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(tex, fragTexCoord);

    if(outlined)
    {
    	if(textureColor.r > 0.5)
    	{
			fragColor = vec4(vertColor.rgb, 1);
    	}
    	else
    	{
    		float udx = 1.0 / 1024.0;
    		float udy = 1.0 / 1024.0;

    		int counter = 0;

    		for(int i = 1; i <= outlineSize; i++)
	    	{
	    		float mul = float(i);

	    		vec4 rightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y));
	    		vec4 leftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y));
	    		vec4 upPixel = texture(tex, vec2(fragTexCoord.x, fragTexCoord.y + (udy * mul)));
	    		vec4 downPixel = texture(tex, vec2(fragTexCoord.x, fragTexCoord.y - (udy * mul)));

	    		vec4 upRightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y + (udy * mul)));
	    		vec4 upLeftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y + (udy * mul)));
	    		vec4 downRightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y - (udy * mul)));
	    		vec4 downLeftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y - (udy * mul)));

	    		if(rightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}    

	    		if(leftPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(upPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downPixel.r > 0.5)
	    		{
	    			counter++;
	    		}   	

	    		if(upRightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}    

	    		if(upLeftPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downRightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downLeftPixel.r > 0.5)
	    		{
	    			counter++;
	    		}  
    		}

    		

    		if(counter > 0)
    		{
				fragColor = vec4(0, 0, 0, 1);
    		}
    		else
    		{
				fragColor = vec4(0, 0, 0, 0);
    		}	
    		/*

	    	//counter = 0;
	    	{

	    		mul = float(2);

	    		vec4 rightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y));
	    		vec4 leftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y));
	    		vec4 upPixel = texture(tex, vec2(fragTexCoord.x, fragTexCoord.y + (udy * mul)));
	    		vec4 downPixel = texture(tex, vec2(fragTexCoord.x, fragTexCoord.y - (udy * mul)));

	    		vec4 upRightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y + (udy * mul)));
	    		vec4 upLeftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y + (udy * mul)));
	    		vec4 downRightPixel = texture(tex, vec2(fragTexCoord.x + (udx * mul), fragTexCoord.y - (udy * mul)));
	    		vec4 downLeftPixel = texture(tex, vec2(fragTexCoord.x - (udx * mul), fragTexCoord.y - (udy * mul)));

	    		if(rightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}    

	    		if(leftPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(upPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downPixel.r > 0.5)
	    		{
	    			counter++;
	    		}   	

	    		if(upRightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}    

	    		if(upLeftPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downRightPixel.r > 0.5)
	    		{
	    			counter++;
	    		}

	    		if(downLeftPixel.r > 0.5)
	    		{
	    			counter++;
	    		} 
	    	} 
	    	*/
    	}
    }
    else
    {
    	if(textureColor.r > 0.5)
    	{
			fragColor = vec4(vertColor.rgb, 1);
    	}
		else
		{
			fragColor = vec4(0, 0, 0, 0);
		}
    }
}