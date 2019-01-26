using System;
using System.Linq;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Windows.Speech;
using UnityStandardAssets.CrossPlatformInput;
using UnityEngine.EventSystems;

namespace UnityStandardAssets.Vehicles.Car
{
    public class CarUserControl : MonoBehaviour
    {

        public delegate void MoveAction();
        public static event MoveAction OnAction;

        private void FixedUpdate()
        {
            if (OnAction != null)
            {
                OnAction();
            }
        }
    }
}
