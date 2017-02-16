package org.cocos2dx.cpp;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import java.util.List;

import org.cocos2dx.cpp.util.IabHelper;
import org.cocos2dx.cpp.util.IabResult;
import org.cocos2dx.cpp.util.Inventory;
import org.cocos2dx.cpp.util.Purchase;

/**
 * Created by truongpd on 6/20/2016.
 */
public class IwinIapHelper {

    public static String TAG = "IwinIapHelper";
    private static Activity _activity;
    private static IabHelper mHelper;
    private static final int REQUEST_CODE = 1302;


    public static void init(final Activity activity)
    {
        _activity = activity;


        mHelper = new IabHelper(_activity, "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAkQNP6ZM/Ydid/PojmJhKEhPSSSN9HEIDTXSUFFgAVRFGE9QVApbyYX1DbPSvE2Fk/T9XXMxDZQ0l3zTgm+mbCzsuv0Gw1rh/HQ6twr+2X1jJyxFyblFaeiqX909PmWNFC4dm5mEEmEDbZ/PT+OepUgqyJB5putt6zBOPzG7lLCnD2TRkXw9ze+L/roxdknc9RPw3tmMtXCxSktib31Z9vUo0rRmr5eEZtSYCbfXeRrXXwwMFL7xbpbEEK1UaDUeH2c3QMUwN+ji6/pA2CrWTzIJP1hAaUzH3/Uv3fBhZofeyoYqb87D6xftHf6Ir29FTYtaS4jVz6auyiXLjpAd2IQIDAQAB");

        // enable debug logging (for a production application, you should set this to false).
        mHelper.enableDebugLogging(true);
        final IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
            public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
                Log.d(TAG, "Query inventory finished.");

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null) return;

                // Is it a failure?
                if (result.isFailure()) {
                    complain("Failed to query inventory: " + result.getMessage());
                    return;
                }

                Log.d(TAG, "Query inventory was successful." );

            /*
             * Check for items we own. Notice that for each purchase, we check
             * the developer payload to see if it's correct! See
             * verifyDeveloperPayload().
             */
                List<Purchase> list_purchases = inventory.getAllPurchases();
                for(int i = 0; i < list_purchases.size(); i++)
                {
                    Purchase purchase = list_purchases.get(i);
                    mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                }
                // Check for gas delivery -- if we own gas, we should fill up the tank immediately
//                Purchase gasPurchase = inventory.getPurchase(SKU_GAS);
//                if (gasPurchase != null && verifyDeveloperPayload(gasPurchase)) {
//                    Log.d(TAG, "We have gas. Consuming it.");
//                    mHelper.consumeAsync(inventory.getPurchase(SKU_GAS), mConsumeFinishedListener);
//                    return;
//                }



            }
        };
        // Start setup. This is asynchronous and the specified listener
        // will be called once setup completes.

        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {


                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    return;
                }

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null) return;
                Log.i(TAG, "Setup Success");
                // IAB is fully set up. Now, let's get an inventory of stuff we own.

                mHelper.queryInventoryAsync(mGotInventoryListener);
            }
        });



    }

    public static boolean onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if(mHelper == null) return false;
        return mHelper.handleActivityResult(requestCode, resultCode, data);
    }

    public static boolean verifyDeveloperPayload(Purchase p) {
        String payload = p.getDeveloperPayload();

        return true;
    }
    static IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.d(TAG, "Purchase finished: " + result + ", purchase: " + purchase);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            if (result.isFailure()) {
                complain("Error purchasing: " + result);
                nativePurchaseFailed(result.getMessage());
                return;
            }
            else
            {
                if (!verifyDeveloperPayload(purchase)) {
                    complain("Error purchasing. Authenticity verification failed.");
                    return;
                }

                mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                System.out.println("AAAAAAAAA " + purchase.getOriginalJson());
                System.out.println("AAAAAAAAA " + purchase.getSignature());
                nativePurchaseSuccess(purchase.getOriginalJson(), purchase.getSignature());

            }
        }
    };

    // Called when consumption is complete
    static IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() {
        public void onConsumeFinished(Purchase purchase, IabResult result) {
            Log.d(TAG, "Consumption finished. Purchase: " + purchase + ", result: " + result);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            // We know this is the "gas" sku because it's the only one we consume,
            // so we don't check which sku was consumed. If you have more than one
            // sku, you probably should check...
            if (result.isSuccess()) {
                // successfully consumed, so we apply the effects of the item in our
                // game world's logic, which in our case means filling the gas tank a bit
                Log.d(TAG, "Consumption successful. Provisioning.");
            }
            else {
                complain("Error while consuming: " + result);
            }
            Log.d(TAG, "End consumption flow.");
        }
    };
    public static void complain(String message) {
        Log.e(TAG, "**** TrivialDrive Error: " + message);
    }


    public static void onDestroy(){
        if(mHelper != null){
            mHelper.dispose();
            mHelper = null;
        }
    }


    public static void purchase(String sku, String payload)
    {
        mHelper.launchPurchaseFlow(_activity, sku, REQUEST_CODE, mPurchaseFinishedListener, payload);
    }

    private static native void nativePurchaseSuccess(final String transactionId, final String payload);
    private static native void nativePurchaseFailed(final String msg);
}
