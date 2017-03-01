package imageconverter;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;

import javax.swing.*;

import javax.imageio.ImageIO;

public class ImageConverter 
{	
	public static void resize(String inputImagePath, String outputImagePath, int scaledWidth, int scaledHeight) throws IOException{
		File imputFile = new File(inputImagePath);
		BufferedImage inputImage = ImageIO.read(imputFile);
		BufferedImage outputImage = new BufferedImage(scaledWidth, scaledHeight, inputImage.getType());
		
		Graphics2D scaledImage = outputImage.createGraphics();
		scaledImage.drawImage(inputImage, 0, 0, scaledWidth, scaledHeight, null);
		scaledImage.dispose();
		
		//get the format of the image
		String format = outputImagePath.substring(outputImagePath.lastIndexOf(".") + 1);
		ImageIO.write(outputImage, format, new File(outputImagePath));
	}
	
	public static int scalingCalculation(int originalWidth, int originalHeight, double scaledWidth){
		double scaling = originalWidth/scaledWidth;
		int scaledHeight = (int)(originalHeight/scaling);
		return scaledHeight;
	}	
	
	public static void imageProcessing(String inputImagePath, String outputImagePath, String outputBinaryPath, double scaledWidth){
		//resize the image first
		BufferedImage image;
		try{
			image = ImageIO.read(new File(inputImagePath));
			
			int scaledHeight = scalingCalculation(image.getWidth(), image.getHeight(), scaledWidth);
			resize(inputImagePath, outputImagePath, (int)scaledWidth, scaledHeight);
		}
		catch(IOException e){
			System.out.println("Error: invalid path on original input image");
			e.printStackTrace();
		}
		
		//convert image to binary stream
		try{
			image = ImageIO.read(new File(outputImagePath));
			PrintStream output = new PrintStream(new FileOutputStream(outputBinaryPath));
			System.setOut(output);
			byte[][]pixels = new byte[image.getWidth()][];
			
			for(int x = 0; x < image.getWidth(); x++){
				pixels[x] = new byte[image.getHeight()];
				
				for(int y = 0; y < image.getHeight(); y++){
					int flag = 0;
					if((image.getRGB(x, y) & 0xFF) >= 127)
						flag++;
					if(((image.getRGB(x, y) >> 2) & 0xFF) >= 127)
						flag++;
					if(((image.getRGB(x, y) >> 4) & 0xFF) >= 127)
						flag++;
					if(flag >= 2)
						pixels[x][y] = 1;
					else
						pixels[x][y] = 0;
					//pixels[x][y] = (byte)(image.getRGB(x, y) == 0xFFFFFFFF ? 0 : 1);
				}
			}
			
			for(int j = 0; j < image.getHeight(); j++){
				for(int i = 0; i < image.getWidth(); i++){
					System.out.print(pixels[i][j]);
				}
				System.out.print("\n");		
			}
		}
		catch(IOException e){
			System.out.println("Error: invalid path on resized image");
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		JFrame.setDefaultLookAndFeelDecorated(true);
	    JDialog.setDefaultLookAndFeelDecorated(true);
	    JFrame frame = new JFrame("Image Conversion");
	    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    frame.setLocationRelativeTo(null);
	    JButton openButton = new JButton("Open File");
	    openButton.addActionListener(new ActionListener() {
	      public void actionPerformed(ActionEvent e) {
	        JFileChooser fileChooser = new JFileChooser();
	        int returnValue = fileChooser.showOpenDialog(null);
	        if (returnValue == JFileChooser.APPROVE_OPTION) {
	          File selectedFile = fileChooser.getSelectedFile();
	          //System.out.println(selectedFile.getPath());
	          String inputImagePath = selectedFile.getPath();
	  		  int index = inputImagePath.lastIndexOf(".");
			  String outputImagePath = inputImagePath.substring(0, index) + "-resized.bmp";
			  String outputBinaryPath = inputImagePath.substring(0, index) + ".txt";
	          double scaledWidth = 24.0;
	          //0 for yes, 1 for no, 2 for cancel
	          int selection = JOptionPane.showConfirmDialog(frame, "Do you want to convert this image to binary text?");
			  if(selection == 0){
				  imageProcessing(inputImagePath, outputImagePath, outputBinaryPath, scaledWidth);
				  JOptionPane.showMessageDialog(frame, "Image converted successfully.");
			  }
	        }
	      }
	    });
	    frame.add(openButton);
	    frame.pack();
	    frame.setVisible(true);
	    
		//String inputImagePath = "C:\\Users\\User\\Documents\\imageProcessing\\helix_1024.jpg";
		//String outputImagePath = "C:\\Users\\User\\Documents\\imageProcessing\\helix_1024-resized.bmp";
		//String outputBinaryPath = "C:\\Users\\User\\Documents\\imageProcessing\\helix_1024.txt";
		//double scaledWidth = 24.0;
		//imageProcessing(inputImagePath, outputImagePath, outputBinaryPath, scaledWidth);
	}
}
