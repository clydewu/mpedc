using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace EDC.UC
{
    public partial class noFindData : System.Web.UI.UserControl
    {
        protected void Page_Load(object sender, EventArgs e)
        {

        }
        public bool Visible
        {
            get
            {
                return panNoFind.Visible;
            }
            set
            {
                panNoFind.Visible = value;
            }
        }

        public string Text
        {
            set
            {
                labMsg.Text = value;
            }
        }
    }
}