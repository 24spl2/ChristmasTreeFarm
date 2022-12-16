# ChristmasTreeFarm
Do you want to plant glass pine trees from the comfort of your chair? I sure did!


At Walmart, you can buy glowing Christmas Tree lights, and after my Graphics Class, I wanted to create the effect of glass and light. Turns out this is extremely difficult to do, so I explored the concept of MatCap. MatCap is when you map vertex points of a image onto a mesh shape. So I found specialized MatCap images of glass and mapped them onto cylinders. These Cylinders were then used to "grow" the trees. I used inspiration from L systems to create the effect of "growth" however, L systems look more like decidous trees whereas I wanted pine trees.

In order to work the farm, you have a red point on the screen---he is named Santa. You can move Santa around using the 'w' (forward), 's' (backward), 'a' (left), and 'd' (right) controls. 

Plant trees by pressing 'j'. To removed trees press 'k'.
For customizable fun, numbers 1-5 have different MatCap images attatched to them. This means that each number will change the shine and color of the tree. 

Trees can only be planted in the grid. However, if you do not want to see the grid press '-' / '+'

Overall, this program is not fast. Graphics in general run on the slower side of things because one updates the entire frame about 30 times a second (for my tree simulation). My next improvement would be to build and draw the trees faster so the user experience has less lag time.
