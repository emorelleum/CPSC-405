public class Cheerleader implements Runnable {

    public static void main(String args[]) {
        Thread t1 = null;
        Thread t2 = null;
        Thread t3 = null;
        try {
            t1 = new Thread(new Cheerleader("Redskins",100));
            t2 = new Thread(new Cheerleader("Cowboys",200));
            t3 = new Thread(new Cheerleader("Steelers",50));
            t1.start();
            t2.start();
            t3.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            t3.join();
        } catch (InterruptedException e) {
            System.out.println("I was waiting on 3 and got whacked...");
        }
        t2.interrupt();
    }

    private String team;
    private int delayMillis;

    public Cheerleader(String team, int delayMillis) {
        this.team = team;
        this.delayMillis = delayMillis;
    }

    public void run() {
        for (int i=0; i<5; i++) {
            try {
                Thread.currentThread().sleep(delayMillis);
            } catch (InterruptedException e) {
                System.out.println("FIRE!!!!");
            }
            System.out.println("Go " + team + "!!");
        }    
    }
}