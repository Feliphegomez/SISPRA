//////////////////////////////////////////////////////////////////////////////////////
///
///  Pin Protector Shell for Arduino Nano and compatibles
///
///  This project generated a shell around an Arduino Nano or Nano compatible
///  which protects all pins from being exposed to accidental shorting with tools
///  or other metallic objects on the work bench.
///
///  Right angle pin headers mounted on the upper side of the Nano PCB are used 
///  instead of the straight headers normally used. Two 15-pin single row and one
///  3x2-pin double row pin header arrays are needed.  This way, all connections
///  to he Nano are horizontal and are safely "hidden" between the upper and lower part
///  of the shell. 
///
///  The so covered horizontal pin connector increade the surface of the Nano shell,
///  so that there is enough space on the top shell for a sticker which identifies
///  each pin in a nicely readable size.
///
///  The top of the shell has a coutout for the 4 LEDs and has an integrated RESET
///  button which is printed in-situ and held in position by its waisted shape.
///
///  Given that there are several variants of the Nano design which essentially differ
///  slightly in the placement of the LEDs and the RESET button, so their longitudinal
///  position (measured in mm from the USB end of the PCB) is parametrized and can
///  easily adapted when needed. 
///
///  In addition to this SCAD file, there is also a corresponding PostScript file
///  which generates the labels to stick on the top part of the shell.
///  
//////////////////////////////////////////////////////////////////////////////////////
///
///  2015-06-13 Heinz Spiess, Switzerland
///
///  released under Creative Commons - Attribution - Share Alike licence (CC BY-SA)
//////////////////////////////////////////////////////////////////////////////////////

ew = 0.56;
eh = 0.252;

module chamfered_cube(size,d=1){
   hull(){
     translate([d,d,0])cube(size-2*[d,d,0]);
     translate([0,d,d])cube(size-2*[0,d,d]);
     translate([d,0,d])cube(size-2*[d,0,d]);
   }
}


// build a cylinder with chamfered edges
module chamfered_cylinder(r=0,r1=0,r2=0,h=0,d=1){
   hull(){
      translate([0,0,d])cylinder(r1=(r?r:r1),r2=(r>0?r:r2),h=h-2*d);
      cylinder(r1=(r?r:r1)-d,r2=(r>0?r:r2)-d,h=h);
   }
}


module nanoshell(
   W = .7*25.4+0.5,
   L = 44,
   H1 = 6,
   H2 = 3,
   pins = 15,
   wall = [6.4,2.3,2.1],
   all = false,
   ledx = 10, 
   ledy = 30,
   resety = 25,
   resetz = 4.5,
   resetr = 2,
   upper = false,
   lower = false,
   case = false,
   move = true,
   eps = -0.2,
){
   usb = [8,9,4];
   isp = [13,8.5,6.5];
   ispk = [2.5,8,7.5];
   Wo = 2*wall[0]+W;
   Ho = 2*wall[2]+H1+H2;
   Lo = wall[1]+L+isp[1];

module case(){
   // reset knob
   translate([Wo/2,wall[1]+resety,Ho-wall[2]-resetz-0.1])rotate(45){
           cylinder(r=resetr,h=2+wall[2],$fn=4);
           cylinder(r2=resetr,r1=resetr+1,h=resetz-1-2,$fn=4);
   }
   difference(){
     // main body
     chamfered_cube([2*wall[0]+W,wall[1]+L+isp[1],2*wall[2]+H1+H2]);
     translate([Wo/2,wall[1]+resety,Ho-wall[2]-2-0.1])rotate(45){
           cylinder(r1=1.5,h=2,$fn=4);
     }
     // main cutout
     difference(){
         translate(wall) cube([W,L,Ho-2*wall[2]-0.7]);
         translate([Wo/2,wall[1]+resety,Ho-wall[2]-2-0.1])
	    cylinder(r1=3.5,r2=6,h=2.5);
     }
     // pin cutout
     translate([Wo/2,0,0])for(sx=[-1,1])scale([sx,1,1])
        translate([-Wo/2-1, wall[1]+L/2-pins/2*2.54,wall[2]+H1])
           cube([wall[0]+1.01,pins*2.54,H2]);
     // mini USB cutout
     translate([wall[0]+W/2-usb[0]/2,-1,wall[2]+H1-2.54])
        cube(usb);
     // 3x2pin ISP header main cutout
     translate([wall[0]+W/2-isp[0]/2,wall[1]+L-1,wall[2]+H1-4])
        cube(isp+[0,2,0]);
     // 3x2pin ISP header key cutout
     translate([wall[0]+W/2-ispk[0]/2,wall[1]+L-1,wall[2]+H1-3-2])
        cube(ispk+[0,2,0]);
     // LED cutout
     translate([Wo/2-ledx/2,wall[1]+ledy,Ho-wall[2]-0.1-0.7]){
        translate([0,0,-H1/2])cube([ledx,2,wall[2]+H1/2]);
        hull(){
           cube([ledx,2,.1]);
           translate([-wall[2],-wall[2],wall[2]+0.7])cube([ledx+2*wall[2],2+2*wall[2],.2]);
        }
     }
     // Reset knob
     translate([Wo/2,wall[1]+resety,Ho-wall[2]-0.1])rotate(45){
        difference(){
           cylinder(r1=resetr+ew,r2=resetr+wall[2]/2+ew,h=wall[2]+0.2,$fn=4);
           cylinder(r1=resetr,r2=resetr+wall[2]/2,h=wall[2]+0.2,$fn=4);
	}
     }
     translate([Wo/2,wall[1]+resety,Ho-wall[2]-2-0.1])rotate(45){
        difference(){
           cylinder(r=resetr+ew,h=wall[2]+0.2,$fn=4);
           cylinder(r=resetr,h=wall[2]+0.2,$fn=4);
	}
     }
   }
}

module cuthalf(gap=0){
   difference(){
      // main lower half
      translate([-1,-1,-1]) cube([Wo+2,Lo+2,wall[2]+H1+1.51-gap]);
      // reset knob
      translate([Wo/2,wall[1]+resety,wall[2]+0.1])
         cylinder(r=5,h=Ho-wall[2]-0.11);
      // clips on USB side
      translate([Wo/2,0,0])for(sx=[-1,1])scale([sx,1,1]){
         translate([usb[0]/2-0.01-gap,-1,-0.1]){
	    cube([Wo/2-usb[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,Ho+2]);
            hull(){
	       translate([0,0,wall[2]])
                  cube([Wo/2-usb[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,Ho-wall[2]]);
               translate([0,0,Ho])
	          cube([Wo/2-usb[0]/2-wall[0]/2+2*gap,wall[1]+5.01+gap,1]);
            }
	    hull(){
	       cube([Wo/2-usb[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,wall[2]+0.1]);
	       translate([0,0,0.1+wall[2]/2-0.05])
	       cube([Wo/2-usb[0]/2-wall[0]/2+2*gap,wall[1]+1.01+wall[2]/2+gap,0.1]);
	    }
         }
      }
      // clips on ISP side
      translate([Wo/2,Lo,0])rotate(180)for(sx=[-1,1])scale([sx,1,1]){
         translate([isp[0]/2-0.01-gap,-1,-0.1]){
	    cube([Wo/2-isp[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,Ho+2]);
            hull(){
	       translate([0,0,wall[2]])
                  cube([Wo/2-isp[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,Ho-wall[2]]);
               translate([0,0,Ho])
	          cube([Wo/2-isp[0]/2-wall[0]/2+2*gap,wall[1]+7.01+gap,1]);
            }
	    hull(){
	       cube([Wo/2-isp[0]/2-wall[0]/2+2*gap,wall[1]+1.01+gap,wall[2]+0.1]);
	       translate([0,0,0.1+wall[2]/2-0.05])
	       cube([Wo/2-isp[0]/2-wall[0]/2+2*gap,wall[1]+1.01+wall[2]/2+gap,0.1]);
	    }
         }
      }
   }

}

module lower(eps=0){
   intersection(){
      case();
      cuthalf(gap=eps);
   }
}

module upper(eps=0){
   translate([0,0,move?Ho:0])rotate([0,move?180:0,0])difference(){
      case();
      cuthalf(gap=eps);
   }
}
     if(case){
        lower(eps=0);
        upper(eps=eps);
     }
     if(lower) lower(eps=0);
     if(upper) upper(eps=eps);
}

module NanoShellR25L30(){ // AUTO_MAKE_STL
   translate([-3,0,0])nanoshell(upper=true,eps=-0.2,resety=25,ledy=30);
   nanoshell(lower=true,resety=25,ledy=30);
}

module NanoShellR24L29(){ // AUTO_MAKE_STL
   translate([-3,0,0])nanoshell(upper=true,eps=-0.2,resety=24,ledy=29);
   nanoshell(lower=true,resety=24,ledy=29);
}

module NanoShellR26L32(){ // AUTO_MAKE_STL
   translate([-3,0,0])nanoshell(upper=true,eps=-0.2,resety=26,ledy=32);
   nanoshell(lower=true,resety=26,ledy=32);
}

translate([-10,0,0])nanoshell(upper=true,eps=-0.2);
//nanoshell(upper=true,move=false,eps=-0.2);
nanoshell(lower=true);
