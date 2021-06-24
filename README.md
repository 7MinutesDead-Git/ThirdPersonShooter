# How did I figure out how to make that fancy ricochet mechanic?  
Well let me show and tell you!  
https://www.youtube.com/watch?v=N-jRAOw7MNw  
*(Sound effects were made in Ableton Live, something I am an expert at from all my time writing and producing music)*

First we learn how to do a line trace to hit an object/point and also spawn a particle effect. I found and altered an impact effect that had a sort of liquid dramatically explode from one angle. But since the particle spawns at random rotations, sometimes the liquid effect would gush out from what we hit, and other times it would go sideways or completely disappear behind the wall.

You can specify the rotation when you spawn the emitter. So I wondered if I could get a rotation based on the surface we hit. I *vaguely* remembered normals had something to do with surfaces, and in our "**FHitResult** Hit" variable, after using Rider to scroll through the list of suggested members, there just happens to be a member variable for it named **ImpactNormal**. That might be where I want to start.

So I repeated that step and used Rider to suggest to me more members on ImpactNormal, because I could see it was some type of Vector but I don't know what "*FVector_NetQuantizeNormal*" actually means. I noticed that ImpactNormal had a Rotation() method, so that's exactly what I could use to set the rotation of the particle emitter.

So at that point I had a particle spawning where it always shot outwards from the surface it hit. But then I thought well, if I'm hitting a surface at an angle shouldn't the particle really be bouncing at a mirrored angle away from me, like a ricochet?

How do I get the *mirror* of the impact direction of our shot, and use the **ImpactNormal** as a surface? What does that even mean for vectors? How do you mirror vectors? I don't know any of this. I barely understand what vectors are!

So naturally I ran through a [short math article online](https://www.mathsisfun.com/algebra/vectors.html) about vectors, and another youtube video [explaining how to mirror vectors coordinates](https://www.youtube.com/watch?v=zyHyhf78T4Y) and get a mirror of a point based on a reflection plane. Well there's the word I really needed: **Reflection**.

So after more Googling I found [FMath::GetReflectionVector](https://docs.unrealengine.com/4.26/en-US/API/Runtime/Core/Math/FMath/GetReflectionVector/), and it allowed me to pass in our direction vector, and use the impact normal as the plane to reflect from.
```cpp
	if (HitSuccess) {
		// If we didn't hit a null actor, then deal damage to it.
		DoDamage(Hit, Direction);

		// Ricochet info that needs to be updated for additional BounceImpact() calls.
		FVector MirrorImpact = FMath::GetReflectionVector(Direction, Hit.ImpactNormal);
		RicochetRotation = MirrorImpact.Rotation();
		RicochetDirection = RicochetRotation.Vector();
		StartLocation = Hit.Location;
```

Then I could just grab its rotation and convert that to a vector with **.Vector()** to get it's direction. Okay, so now we've got a mirrored direction of our impact. Let's go!

To make something look like it bounces and continues on along its new path, I figured you'd have to do an identical line trace but now your end point from your initial shot is your new starting point from that same point you just hit, and your new direction is the mirror of the previous direction.

This is all in a "BounceImpact()" method. How do I pass in subsequent bounces into itself? 

Really you can just call it recursively, starting with the direction and point of your initial player shot, then pass in the new start point and new direction in subsequent calls and calculate the next new point and direction and pass it in again and again until you reach your desired bounces.

So I did that, but it was all completely instantaneous so I couldn't tell how well it was bouncing around. I needed to add some sort of delay before the next recursive call to this function so that it would look like it's actually traveling and bouncing around. I figured you can't just add a delay in the code since it'll pause the entire program, but I did recently learn about Timers and Delegates in UE4, so I did some more Googling.

What I needed was a way to bind our Bounce function to a delegate so we could call it on a timer after a delay that is run on its own thread so it doesn't stop our program that *also* lets us pass in parameters, because we update the parameters for every bounce. I hadn't learned how to delegate functions with parameters yet, so I learned by finding [this exact question asked on UE Answers](https://answers.unrealengine.com/questions/165678/using-settimer-on-a-function-with-parameters.html). I implemented that exactly.

```cpp
// Setup timer delegate so we can make a sort of delayed ricochet bounce by calling this again.
// Recursive calls means updating the parameters/values with the new bind.
RicochetDelegate.BindUFunction(this, FName("BounceImpact"), StartLocation, RicochetDirection);

// Our recursive loop and base case.
if (RicochetBounces < RicochetMaxBounces) {
	RicochetBounces += 1;
	// Slowly speed up the bounces as we go on.
	RicochetBounceDelay *= RicochetBounceSpeedGrowthFactor;
	// Call this function again after a short delay, with updated parameters.
	GetWorld()->GetTimerManager().SetTimer(RicochetTimerHandle, RicochetDelegate, RicochetBounceDelay, false);
}
```
And it worked!

You can check out the [entire relevant cpp file and Bounce method here](https://github.com/yayorbitgum/ThirdPersonShooter/blob/master/Source/Shooter/Weapon.cpp).
